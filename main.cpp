#include <iostream>
#include <string>
using namespace std;

// Define a structure to represent a book
struct Book {
    int ISBN;        // Unique identifier for the book
    string title;    // Title of the book
    string author;   // Author of the book

    // Constructor to initialize a book with ISBN, title, and author
    Book(int isbn, string t, string a) {
        ISBN = isbn;
        title = t;
        author = a;
    }
};

// Define a structure to represent a node in the AVL Tree
struct TreeNode {
    Book* book;        // Pointer to a Book object
    TreeNode* left;    // Pointer to the left child node
    TreeNode* right;   // Pointer to the right child node
    int height;        // Height of the node in the tree

    // Constructor to initialize a node with a book
    TreeNode(Book* b) {
        book = b;
        left = nullptr;
        right = nullptr;
        height = 1; // Initially, height of the node is 1 (leaf node)
    }
};

// Define the AVL Tree class
class AVLTree {
private:
    TreeNode* root; // Pointer to the root of the tree

    // Function to get the height of a node (returns 0 for nullptr)
    int getHeight(TreeNode* node) {
        if(node == nullptr) {
            return 0;
        } else {
            return node->height;
        }
    }

    // Function to calculate the balance factor of a node (difference between heights of left and right subtrees)
    int getBalance(TreeNode* node) {
        if(node == nullptr) {
            return 0;
        } else {
            return getHeight(node->left) - getHeight(node->right);
        }
    }

    // Right rotation to balance the tree (used in left-heavy cases)
    TreeNode* rightRotate(TreeNode* y) {
        TreeNode* x = y->left;        // Make left child the new root
        TreeNode* T2 = x->right;      // Temporarily save the right child of x

        x->right = y;                 // Perform the right rotation
        y->left = T2;                 // Connect the temporary node to the left child of y

        // Update the heights of nodes after rotation
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x; // Return the new root node
    }

    // Left rotation to balance the tree (used in right-heavy cases)
    TreeNode* leftRotate(TreeNode* x) {
        TreeNode* y = x->right;       // Make right child the new root
        TreeNode* T2 = y->left;       // Temporarily save the left child of y

        y->left = x;                  // Perform the left rotation
        x->right = T2;                // Connect the temporary node to the right child of x

        // Update the heights of nodes after rotation
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

        return y; // Return the new root node
    }

    // Function to balance the tree if needed (called after every insertion and deletion)
    TreeNode* balance(TreeNode* node) {
        int balanceFactor = getBalance(node); // Calculate the balance factor of the node

        // Left-heavy case (left child is taller)
        if (balanceFactor > 1) {
            // Left-Right case (left child is right-heavy)
            if (getBalance(node->left) < 0) {
                node->left = leftRotate(node->left); // Perform left rotation on left child
            }
            return rightRotate(node); // Perform right rotation to balance
        }

        // Right-heavy case (right child is taller)
        if (balanceFactor < -1) {
            // Right-Left case (right child is left-heavy)
            if (getBalance(node->right) > 0) {
                node->right = rightRotate(node->right); // Perform right rotation on right child
            }
            return leftRotate(node); // Perform left rotation to balance
        }

        return node; // No balancing needed
    }

    // Function to insert a book into the AVL tree
    TreeNode* insert(TreeNode* node, Book* book) {
        if (!node) return new TreeNode(book); // If the tree is empty, create a new node

        // Perform the regular BST insertion (insert the book by ISBN)
        if (book->ISBN < node->book->ISBN)
            node->left = insert(node->left, book); // Insert into the left subtree
        else if (book->ISBN > node->book->ISBN)
            node->right = insert(node->right, book); // Insert into the right subtree

        // Update the height of the current node after insertion
        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

        // Balance the tree to maintain AVL properties (height difference of child nodes should be <= 1)
        return balance(node);
    }

    // Function to find the node with the minimum ISBN value (used during deletion)
    TreeNode* minNode(TreeNode* node) {
        TreeNode* current = node;
        while (current && current->left) {
            current = current->left; // Keep moving to the leftmost node
        }
        return current; // Return the leftmost (minimum) node
    }

    // Function to delete a book from the AVL tree by ISBN
    TreeNode* deleteNode(TreeNode* root, int ISBN) {
        if (!root) return root; // If the tree is empty, return nullptr

        // Perform regular BST deletion
        if (ISBN < root->book->ISBN)
            root->left = deleteNode(root->left, ISBN); // Search left subtree
        else if (ISBN > root->book->ISBN)
            root->right = deleteNode(root->right, ISBN); // Search right subtree
        else { // This is the node to be deleted
            if (!root->left || !root->right) { // If the node has one or no children
                TreeNode* temp = root->left ? root->left : root->right;
                if (!temp) { // If node has no children, just delete it
                    temp = root;
                    root = nullptr;
                } else { // Copy the content of the non-null child to the node
                    *root = *temp;
                }
                delete temp; // Free the memory
            } else { // If the node has two children
                TreeNode* temp = minNode(root->right); // Find the node with the smallest value in the right subtree
                root->book = temp->book; // Copy the book from the minimum node
                root->right = deleteNode(root->right, temp->book->ISBN); // Delete the node that we copied from the right subtree
            }
        }

        if (!root) return root; // If the tree becomes empty, return nullptr

        // Update the height of the current node after deletion
        root->height = 1 + max(getHeight(root->left), getHeight(root->right));

        // Balance the tree again
        return balance(root);
    }

    // Inorder traversal to display all books in sorted order
    void inorder(TreeNode* node) {
        if (node) {
            inorder(node->left); // Traverse the left subtree
            cout << "ISBN: " << node->book->ISBN << ", Title: " << node->book->title << ", Author: " << node->book->author << endl;
            inorder(node->right); // Traverse the right subtree
        }
    }

    // Function to search for a book by ISBN
    TreeNode* search(TreeNode* node, int ISBN) {
        if (!node || node->book->ISBN == ISBN) return node; // Return node if found or nullptr if not found
        if (ISBN < node->book->ISBN) return search(node->left, ISBN); // Search in the left subtree
        return search(node->right, ISBN); // Search in the right subtree
    }

    // Function to display books within a given range of ISBNs
    void rangeSearch(TreeNode* node, int low, int high) {
        if (node) {
            if (node->book->ISBN >= low && node->book->ISBN <= high)
                cout << "ISBN: " << node->book->ISBN << ", Title: " << node->book->title << ", Author: " << node->book->author << endl;
            if (node->book->ISBN > low)
                rangeSearch(node->left, low, high); // Search in the left subtree if ISBN is greater than low
            if (node->book->ISBN < high)
                rangeSearch(node->right, low, high); // Search in the right subtree if ISBN is less than high
        }
    }

public:
    // Constructor to initialize an empty AVL tree
    AVLTree() : root(nullptr) {}

    // Add a new book to the tree
    void addBook(int ISBN, string title, string author) {
        Book* book = new Book(ISBN, title, author);
        root = insert(root, book); // Insert the new book into the tree
    }

    // Remove a book from the tree by ISBN
    void removeBook(int ISBN) {
        if (!root) {
            cout << "The tree is empty, no book to remove!" << endl;
            return; // Return if the tree is empty
        }
        root = deleteNode(root, ISBN); // Remove the book and balance the tree
        cout << "Book removed successfully!" << endl;
    }

    // Display all books in sorted order (inorder traversal)
    void displayAllBooks() {
        if (!root) {
            cout << "The tree is empty. No books to display." << endl;
            return; // Return if the tree is empty
        }
        cout << "All Books in sorted order: " << endl;
        inorder(root); // Traverse the tree and display books
    }

    // Search for a book by ISBN and display it
    void displayBook(int ISBN) {
        TreeNode* node = search(root, ISBN); // Search for the book by ISBN
        if (node) {
            cout << "Found Book - ISBN: " << node->book->ISBN << ", Title: " << node->book->title << "|| Author: " << node->book->author << endl;
        } else {
            cout << "Book not found!" << endl; // If book is not found
        }
    }

    // Display books within a given range of ISBNs
    void displayBooksInRange(int low, int high) {
        cout << "Books with ISBN between " << low << " and " << high << ": " << endl;
        rangeSearch(root, low, high); // Search and display books in the given range
    }
};

// Main function to run the bookstore program
int main() {
    AVLTree bookstore;
    int choice;

    // Menu for user to interact with the system
    while (true) {
        cout << "\n-- Bookstore Menu --\n";
        cout << "1. Add a new book\n";
        cout << "2. Display all books\n";
        cout << "3. Search for a book by ISBN\n";
        cout << "4. Remove a book by ISBN\n";
        cout << "5. Display books within a given ISBN range\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        // Add a new book
        if (choice == 1) {
            int ISBN;
            string title, author;
            cout << "Enter ISBN: ";
            cin >> ISBN;
            cin.ignore();  // to consume any leftover newline character from the input buffer
            cout << "Enter title: ";
            getline(cin, title);
            cout << "Enter author: ";
            getline(cin, author);

            bookstore.addBook(ISBN, title, author);
            cout << "Book added successfully!" << endl;

        // Display all books
        } else if (choice == 2) {
            bookstore.displayAllBooks();

        // Search for a book by ISBN
        } else if (choice == 3) {
            int ISBN;
            cout << "Enter ISBN to search: ";
            cin >> ISBN;

            bookstore.displayBook(ISBN);

        // Remove a book by ISBN
        } else if (choice == 4) {
            int ISBN;
            cout << "Enter ISBN to remove: ";
            cin >> ISBN;

            bookstore.removeBook(ISBN);

        // Display books within a range of ISBNs
        } else if (choice == 5) {
            int low, high;
            cout << "Enter low ISBN: ";
            cin >> low;
            cout << "Enter high ISBN: ";
            cin >> high;

            bookstore.displayBooksInRange(low, high);

        // Exit the program
        } else if (choice == 6) {
            cout << "Exiting the program. Goodbye!" << endl;
            break;

        // Handle invalid choices
        } else {
            cout << "Invalid choice! Please try again." << endl;
        }
    }

    return 0;
}
