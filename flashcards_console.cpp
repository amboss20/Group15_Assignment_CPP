#include <iostream>  // Default library to run c++ program
#include <string>    // String functionality
#include <vector>    // Vector function (dynamic memory allocation)
#include <fstream>   // File operation
#include <algorithm> // Added for std::sort

using namespace std;

// Define difficulty level (for spaced repetition)
// HARD --> card often appears again
// MEDIUM --> appears moderately (default level)
// EASY --> card appears occasionally
enum Difficulty { HARD = 0, MEDIUM = 1, EASY = 2 };

// Custom function (shortcut) to print with newline
void print(string str) {
    cout << str << endl;
}

class FlashCard {

// Stores information of each flashcard
private:
    string question;
    string answer;
    Difficulty difficulty;


public:
    // Default difficult level for a card
    FlashCard() { difficulty = MEDIUM; }  

    // Creates new card by obtaining question & answer
    // Obtains a question
    void setQuestion() {
        cout << "Add a question: ";
        getline(cin, question);
    }

    // Obtains the corresponding answer
    void setAnswer() {
        cout << "Add the answer: ";
        getline(cin, answer);
    }

    // Used when loading flashcards from the text file.
    // Loads information of each card
    // To track user progress later in main function
    void loadCardDetails(string q, string a, Difficulty d) {
        question = q;
        answer = a;
        difficulty = d;
    }

    // Getter functions to return the stored data of each card
    string getQuestion()       { return question; }
    string getAnswer()         { return answer; }
    // Returns difficulty enum value
    // Mainly used for sorting (spaced repetition)
    Difficulty getDifficulty() const { return difficulty; }

    // To update the flashcard difficulty after the user gives 
    // feedback during reviewing a card.
    void setDifficulty(Difficulty d) { difficulty = d; }

    // Converts the enum value into a readable string for display.
    string getDifficultyLabel() {
        if (difficulty == HARD)   return "Hard";
        if (difficulty == MEDIUM) return "Medium";
        return "Easy";
    }
};

class UserProgress {
private:
    // Creates flashcard pointer object
    // Aggregation purpose
    FlashCard *card;  

public:
    // Constructor to initialize flashcard object
    UserProgress(FlashCard *c) { card = c; }

    // Function to display user progress
    void progress() {
        cout << "User Progress:" << endl;
        cout << "Question:   " << card->getQuestion() << endl;
        cout << "Difficulty: " << card->getDifficultyLabel() << endl;
        cout << endl;
    }
};

class StudySession {
private:
    FlashCard *card;  // Aggregation

public:
    StudySession(FlashCard *c) { card = c; }

    void showCard() {
        cout << "Question: " << card->getQuestion() << endl;
        cout << endl;
        cout << "Press ENTER to reveal answer...";
        cin.ignore(); // Pauses and waits for the user to hit Enter
        cout << "Answer: " << card->getAnswer() << endl;
        cout << endl;
    }

    void recordFeedback() {
        int feedback = 0;
        cout << "How difficult was this card?" << endl;
        cout << "1. Hard   (will appear first next time)"  << endl;
        cout << "2. Medium (will appear in the middle)"    << endl;
        cout << "3. Easy   (will appear last next time)"   << endl;
        cout << "Enter choice: ";
        cin >> feedback;
        cin.ignore(); // Clean buffer after reading integer

        if (feedback == 1) {
            card->setDifficulty(HARD);
            cout << "Marked as Hard. This card will appear first." << endl;
        } else if (feedback == 2) {
            card->setDifficulty(MEDIUM);
            cout << "Marked as Medium. This card will appear in the middle." << endl;
        } else if (feedback == 3) {
            card->setDifficulty(EASY);
            cout << "Marked as Easy. This card will appear last." << endl;
        } else {
            cout << "Invalid input. Difficulty unchanged." << endl;
        }
    }

    void startSession(int current, int total) {
        cout << endl;
        cout << "---------------------------------" << endl;
        cout << "   REVIEW SESSION  [" << current << "/" << total << "]" << endl;
        cout << "---------------------------------" << endl;

        showCard();
        recordFeedback();
    }
};

class Deck {
private:
    vector<FlashCard> cards; // Dynamic array to hold flashcards

public:
    void addNewCard() {
        FlashCard newCard;
        newCard.setQuestion();
        newCard.setAnswer();
        cards.push_back(newCard);
        print("Card added successfully!");
    }

    vector<FlashCard>& getCards() {
        return cards;
    }

    void clearDeck() {
        cards.clear();
    }

    // Composition: Deck fully owns its FlashCards, so removal is managed here
    void removeCard() {
        if (cards.empty()) {
            print("The deck is empty! Nothing to remove.");
            return;
        }

        cout << "--- Select a Card to Remove ---" << endl;
        for (size_t i = 0; i < cards.size(); i++) {
            cout << i + 1 << ". " << cards[i].getQuestion() << endl;
        }

        int pick = 0;
        cout << "Enter card number to remove: ";
        cin >> pick;
        cin.ignore();

        if (pick < 1 || pick > (int)cards.size()) {
            print("Invalid selection. No card removed.");
            return;
        }

        cards.erase(cards.begin() + pick - 1);
        print("Card removed successfully!");
    }

    void sortByDifficulty() {
        sort(cards.begin(), cards.end(), [](const FlashCard& a, const FlashCard& b) {
            return a.getDifficulty() < b.getDifficulty(); // HARD(0) < MEDIUM(1) < EASY(2)
        });
    }
};

class FileManager {
public:
    static void saveToFile(Deck& deck, string filename) {
        ofstream outFile(filename);

        if (!outFile) {
            print("Error: Could not open file for writing.");
            return;
        }

        vector<FlashCard>& cards = deck.getCards();
        for (size_t i = 0; i < cards.size(); i++) {
            outFile << cards[i].getQuestion() << endl;
            outFile << cards[i].getAnswer() << endl;
            outFile << static_cast<int>(cards[i].getDifficulty()) << endl;
        }

        outFile.close();
        print("Deck saved successfully to " + filename);
    }

    static void loadFromFile(Deck& deck, string filename) {
        ifstream inFile(filename);

        if (!inFile) {
            print("No saved file found. Starting with an empty deck.");
            return;
        }

        deck.clearDeck();

        string qLines, aLines, diffLines;
        while (getline(inFile, qLines) && getline(inFile, aLines) && getline(inFile, diffLines)) {
            // Read the integer string from the file and turn it back into the Enum type
            int diffValue = stoi(diffLines); 
            Difficulty savedDifficulty = static_cast<Difficulty>(diffValue);

            FlashCard loadedCard;
            loadedCard.loadCardDetails(qLines, aLines, savedDifficulty);
            deck.getCards().push_back(loadedCard);
        }

        inFile.close();
        print("Deck loaded successfully from " + filename);
    }
};

int main() {
    // Flashcard deck object
    // Stores all flahscards throughout the program (composition)
    Deck myDeck;
    // Stores the name of the text file used for saving & loading flashcards
    // Stores all information for each card
    string filename = "flashcards.txt";

    // Load saved cards automatically into the deck at startup
    FileManager::loadFromFile(myDeck, filename);

    // Default value for user choice
    // To prevent unwanted behaviour later
    int choice = 0;

    // do-while loop: Creates an infinite loop that repeatedly 
    // displays the main menu until the user chooses to exit.
    do {
        // Display menu for flashcard console interface
        cout << endl;
        print("---------------------------------");
        print("      FLASHCARD CONSOLE          ");
        print("---------------------------------");
        print("1. Add FlashCard");
        print("2. Remove FlashCard");
        print("3. Review Flashcards");
        print("4. View Progress");
        print("5. Save Deck");
        print("6. Exit");
        cout << endl;

        // Read user's menu selection
        cout << "Enter your choice (number): ";
        cin >> choice;
        // Clears the newline character in input buffer
        cin.ignore(); 
        cout << endl;

        // Adds new flashcard
        // Sets corresponding question and answer
        // Saves it into the deck first
        if (choice == 1) {
            cout << "--- Creating a Card ---" << endl;
            myDeck.addNewCard();
        }

        // Displays all questions corresponding to each card
        // Lets the user choose one
        // Removes it from the vector
        else if (choice == 2) {
            myDeck.removeCard();
        }

        // To enter study session / review session
        else if (choice == 3) {
            //  Retrieves all saved cards
            vector<FlashCard>& cards = myDeck.getCards();
            
            // Checks if deck is empty
            // Notifies to add some cards 
            if (cards.empty()) {
                print("The deck is empty! Add some cards first.");

            // If deck have content of the cards
            // Sorts the cards by difficulty from hard to easy
            // Ensures difficult cards appear first
            // Spaced repetition
            } else {
                myDeck.sortByDifficulty();
            
            // A loop to iterate through each flashcard
            // Initiates study / review session
            // Shows question, reveals answer
            // Asks about difficulty and update accordingly
                for (size_t i = 0; i < cards.size(); i++) {
                    StudySession session(&cards[i]);
                    session.startSession(i + 1, cards.size());
                }
            }
        }

        // To view user progress 
        else if (choice == 4) {
            // Retrieves cards from the deck
            vector<FlashCard>& cards = myDeck.getCards();
            
            // Checks if the deck is empty
            if (cards.empty()) {
                print("The deck is empty! No progress to show.");

            // Displays every flashcard's question with its difficulty level
            } else {
                for (size_t i = 0; i < cards.size(); i++) {
                    UserProgress track(&cards[i]);
                    track.progress();
                }
                // Waits until the user presses ENTER before returning to the menu
                cout << "Press ENTER to return to the menu...";
                cin.ignore();
            }
        }

        // Saves all flashcards with their infos respectively
        // Questions, answers, difficulty levels
        // Saves in dedicated text file
        else if (choice == 5) {
            cout << "Saving your deck..." << endl;
            FileManager::saveToFile(myDeck, filename);
        }

        // Prints a message and exit the console
        else if (choice == 6) {
            print("Goodbye!");
            return 0;
        }

        // Prints a warning if invalid choice is selected
        else {
            print("Invalid choice. Please select 1-6.");
        }
    
    // Keeps the program running until user chooses to exit
    } while (true);

    return 0;
}
