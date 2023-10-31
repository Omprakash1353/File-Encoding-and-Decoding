// DESCRIPTION : This C++ Code contains the text file Compression (encoding) and Decompression (decoding) using Huffman's Algorithm
// where a complete binary tree is made and on that basis the binary pattern for all similar characters are assigned
// IMPLEMENTATION : Need a text file with data and then it can be compressed
// USE : Memory efficient and file encryption

#include <bits/stdc++.h>
using namespace std;

//Defining Huffman Tree Node
struct Node {
    char data;
    unsigned freq;
    string code;
    Node *left, *right;
    Node() {
        left = right = NULL;
    }
};

class huffman {
    private:

        vector <Node*> arr;
        fstream inFile, outFile;
        string inFileName, outFileName;
        Node *root;

        class Compare {
            public:
                bool operator() (Node* l, Node* r) {
                    return l->freq > r->freq;
                }
        };

        // To store Node* in container of type Node* on the basis of their frequency
        priority_queue <Node*, vector<Node*>, Compare> minHeap;

        //Initializing a vector of tree nodes representing character's ascii value and initializing its frequency with 0
        void createArr() {
            for (int i = 0; i < 128; i++) {
                arr.push_back(new Node());
                arr[i]->data = i;
                arr[i]->freq = 0;
            }
        }

        //Traversing the constructed tree to generate huffman codes of each present character
        void traverse(Node* root, string str) {
            // If root is a leaf node
            if (root->left == NULL && root->right == NULL) {
                root->code = str;
                return;
            }
            // Append '0' in the left root node pattern
            traverse(root->left, str + '0');
            // Append '1' in the right root node pattern
            traverse(root->right, str + '1');
        }

        //Function to convert binary string to its equivalent decimal value
        int binToDec(string inStr) {
            int res = 0;
            for (auto c : inStr) {
                res = res * 2 + c - '0';
            }
            return res;
        }

        //Function to convert a decimal number to its equivalent binary string
        string decToBin(int inNum) {
            string temp = "", res = "";
            while (inNum > 0) {
                temp += (inNum % 2 + '0');
                inNum /= 2;
            }
            res.append(8 - temp.length(), '0');
            for (int i = temp.length() - 1; i >= 0; i--) {
                res += temp[i];
            }
            return res;
        }

        //Reconstructing the Huffman tree while Decoding the file
        void buildTree(char a_code, string& path) {
            Node* curr = root;
            for (int i = 0; i < path.length(); i++) {
                if (path[i] == '0') {
                    if (curr->left == NULL) {
                        curr->left = new Node();
                    }
                    curr = curr->left;
                }
                else if (path[i] == '1') {
                    if (curr->right == NULL) {
                        curr->right = new Node();
                    }
                    curr = curr->right;
                }
            }
            curr->data = a_code;
        }

        // Creating Min Heap of Nodes by frequency of characters in the input file
        void createMinHeap() {
            char id;
            inFile.open(inFileName, ios::in);
            inFile.get(id);
            // Incrementing frequency of characters that appear in the input file
            while (!inFile.eof()) {
                arr[id]->freq++;
                inFile.get(id);
            }
            inFile.close();
            // Pushing the Nodes which appear in the file into the priority queue (Min Heap)
            for (int i = 0; i < 128; i++) {
                if (arr[i]->freq > 0) {
                    minHeap.push(arr[i]);
                }
            }
        }

        // Constructing the Huffman tree
        void createTree() {
            // Creating Huffman Tree with the Min Heap created earlier
            Node *left, *right;
            priority_queue <Node*, vector<Node*>, Compare> tempPQ(minHeap);
            while (tempPQ.size() != 1) {
                left = tempPQ.top();
                tempPQ.pop();
                right = tempPQ.top();
                tempPQ.pop();
                // Root freq is the addition of left freq and right freq
                root = new Node();
                root->freq = left->freq + right->freq;
                root->left = left;
                root->right = right;
                tempPQ.push(root);
            }
        }
        
        // Generating Huffman codes
        void createCodes() {
            // Traversing the Huffman Tree and assigning specific codes to each character
            traverse(root, "");
        }
        
        // Saving Huffman Encoded File
        void saveEncodedFile() {
            // Saving encoded file
            inFile.open(inFileName, ios::in);
            outFile.open(outFileName, ios::out | ios::binary);
            string in = "";
            string s = "";
            char id;
            // Saving the meta data (huffman tree)
            in += (char)minHeap.size();
            priority_queue <Node*, vector<Node*>, Compare> tempPQ(minHeap);
            while (!tempPQ.empty()) {
                Node* curr = tempPQ.top(); 
                in += curr->data;
                //Saving 16 decimal values representing code of curr->data
                s.assign(127 - curr->code.length(), '0');
                s += '1';
                s += curr->code;
                //Saving decimal values of every 8-bit binary code 
                in += (char)binToDec(s.substr(0, 8));
                for (int i = 0; i < 15; i++) {
                    s = s.substr(8);
                    in += (char)binToDec(s.substr(0, 8));
                }
                tempPQ.pop();
            }
            s.clear();
            //Saving codes of every charachter appearing in the input file
            inFile.get(id);
            while (!inFile.eof()) {
                s += arr[id]->code;
                //Saving decimal values of every 8-bit binary code
                while (s.length() > 8) {
                    in += (char)binToDec(s.substr(0, 8));
                    s = s.substr(8);
                }
                inFile.get(id);
            }
            //Finally if bits remaining are less than 8, append 0's
            int count = 8 - s.length();
        	if (s.length() < 8) {
        		s.append(count, '0');
        	}
        	in += (char)binToDec(s);	
            //append count of appended 0's
            in += (char)count;
            //write the in string to the output file
        	outFile.write(in.c_str(), in.size());
        	inFile.close();
        	outFile.close();
        }
        
        //Saving Decoded File to obtain the original File
        void saveDecodedFile() {
            inFile.open(inFileName, ios::in | ios::binary);
            outFile.open(outFileName, ios::out);
            unsigned char size;
            inFile.read(reinterpret_cast<char*>(&size), 1);
            //Reading count at the end of the file which is number of bits appended to make final value 8-bit
            inFile.seekg(-1, ios::end);
            char count0;
            inFile.read(&count0, 1);
            //Ignoring the meta data (huffman tree) (1 + 17 * size) and reading remaining file
            inFile.seekg(1 + 17 * size, ios::beg);
            vector<unsigned char> text;
            unsigned char textseg;
            inFile.read(reinterpret_cast<char*>(&textseg), 1);
            while (!inFile.eof()) {
                text.push_back(textseg);
                inFile.read(reinterpret_cast<char*>(&textseg), 1);
            }
            Node *curr = root;
            string path;
            for (int i = 0; i < text.size() - 1; i++) {
                //Converting decimal number to its equivalent 8-bit binary code
                path = decToBin(text[i]);
                if (i == text.size() - 2) {
                    path = path.substr(0, 8 - count0);
                }
                //Traversing huffman tree and appending resultant data to the file
                for (int j = 0; j < path.size(); j++) {
                    if (path[j] == '0') {
                        curr = curr->left;
                    }
                    else {
                        curr = curr->right;
                    }
                    if (curr->left == NULL && curr->right == NULL) {
                        outFile.put(curr->data);
                        curr = root;
                    }
                }
            }
            inFile.close();
            outFile.close();
        }
        
        //Reading the file to reconstruct the Huffman tree
        void getTree() {
            inFile.open(inFileName, ios::in | ios::binary);
            //Reading size of MinHeap
            unsigned char size;
            inFile.read(reinterpret_cast<char*>(&size), 1);
            root = new Node();
            //next size * (1 + 16) characters contain (char)data and (string)code[in decimal]
            for(int i = 0; i < size; i++) {
                char aCode;
                unsigned char hCodeC[16];
                inFile.read(&aCode, 1);
                inFile.read(reinterpret_cast<char*>(hCodeC), 16);
                //converting decimal characters into their binary equivalent to obtain code
                string hCodeStr = "";
                for (int i = 0; i < 16; i++) {
                    hCodeStr += decToBin(hCodeC[i]);
                }
                //Removing padding by ignoring first (127 - curr->code.length()) '0's and next '1' character
                int j = 0;
                while (hCodeStr[j] == '0') {
                    j++;
                }
                hCodeStr = hCodeStr.substr(j+1);
                //Adding node with aCode data and hCodeStr string to the huffman tree
                buildTree(aCode, hCodeStr);
            }
            inFile.close();
        }

    public:

        // Constructor
        huffman(string inFileName, string outFileName) {
            this->inFileName = inFileName;
            this->outFileName = outFileName;
            createArr();
        }

        // Compressing input file
        void compress() {
            createMinHeap();
            createTree();
            createCodes();
            saveEncodedFile();
        }

        // Decrompressing input file
        void decompress() {
            getTree();
            saveDecodedFile();
        }
};

int main() {
    cout << "1. Compress File\n";
    cout << "2. Decompress File\n";
    cout << "Enter your choice : ";
    int choice;
    string inputFile, outputFile;
    cin >> choice;
    if(choice == 1) {
        cout << "Enter txt FileName which you want to compress : ";
        cin >> inputFile;
        cout << "Suggest a compressed File Name : ";
        cin >> outputFile;
        ifstream file1(inputFile);
        if(file1.is_open()) {
            huffman huf(inputFile, outputFile);
            huf.compress();
            cout << "File Successfully compressed\n";
        }
        else{
            cout << "File Not Found\n";
        }
    }
    else if(choice == 2) {
        cout << "Enter compressed File Name with same algorithm : ";
        cin >> inputFile;
        cout << "Suggest a decompressed File Name : ";
        cin >> outputFile;
        ifstream file2(inputFile);
        if(file2.is_open()) {
            huffman huf(inputFile, outputFile);
            huf.decompress();
            cout << "File Successfully decompressed\n";
        }
        else{
            cout << "File Not Found\n";
        }
    }
    else{
        cout << "Invalid Input\n";
    }
    return 0;
}
