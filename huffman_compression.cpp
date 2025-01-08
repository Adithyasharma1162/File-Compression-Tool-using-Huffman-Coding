#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <bitset>

using namespace std;

// Node for Huffman Tree
struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue
struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

// Traverse the Huffman Tree to generate codes
void generateCodes(Node* root, const string& str, unordered_map<char, string>& huffmanCode) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    generateCodes(root->left, str + "0", huffmanCode);
    generateCodes(root->right, str + "1", huffmanCode);
}

// Build the Huffman Tree
Node* buildHuffmanTree(const unordered_map<char, int>& freqMap) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    for (auto pair : freqMap) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        int sum = left->freq + right->freq;
        Node* newNode = new Node('\0', sum);
        newNode->left = left;
        newNode->right = right;

        pq.push(newNode);
    }

    return pq.top();
}

// Compress the input file
void compressFile(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::binary);
    if (!inFile.is_open()) {
        cerr << "Error opening input file!" << endl;
        return;
    }

    string text((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    unordered_map<char, int> freqMap;
    for (char ch : text) {
        freqMap[ch]++;
    }

    Node* root = buildHuffmanTree(freqMap);

    unordered_map<char, string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    string compressedText = "";
    for (char ch : text) {
        compressedText += huffmanCode[ch];
    }

    ofstream outFile(outputFile, ios::binary);
    if (!outFile.is_open()) {
        cerr << "Error opening output file!" << endl;
        return;
    }

    // Write codes for decoding later
    for (auto pair : huffmanCode) {
        outFile << pair.first << ':' << pair.second << '\n';
    }
    outFile << "---\n";

    // Write compressed data
    for (size_t i = 0; i < compressedText.size(); i += 8) {
        bitset<8> bits(compressedText.substr(i, 8));
        outFile.put(static_cast<char>(bits.to_ulong()));
    }

    outFile.close();
    cout << "File compressed successfully!" << endl;
}

// Decompress the file
void decompressFile(const string& compressedFile, const string& outputFile) {
    ifstream inFile(compressedFile, ios::binary);
    if (!inFile.is_open()) {
        cerr << "Error opening compressed file!" << endl;
        return;
    }

    unordered_map<string, char> reverseHuffmanCode;
    string line;
    while (getline(inFile, line) && line != "---") {
        if (line.empty()) continue;
        reverseHuffmanCode[line.substr(2)] = line[0];
    }

    string compressedBits = "";
    char byte;
    while (inFile.get(byte)) {
        bitset<8> bits(byte);
        compressedBits += bits.to_string();
    }
    inFile.close();

    string currentCode = "";
    string decompressedText = "";
    for (char bit : compressedBits) {
        currentCode += bit;
        if (reverseHuffmanCode.count(currentCode)) {
            decompressedText += reverseHuffmanCode[currentCode];
            currentCode = "";
        }
    }

    ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        cerr << "Error opening output file!" << endl;
        return;
    }

    outFile << decompressedText;
    outFile.close();
    cout << "File decompressed successfully!" << endl;
}

int main() {
    string inputFile, compressedFile, decompressedFile;

    cout << "Enter input file name: ";
    cin >> inputFile;
    cout << "Enter compressed file name: ";
    cin >> compressedFile;
    cout << "Enter decompressed file name: ";
    cin >> decompressedFile;

    compressFile(inputFile, compressedFile);
    decompressFile(compressedFile, decompressedFile);

    return 0;
}
