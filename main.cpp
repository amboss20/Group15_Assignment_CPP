#include <iostream>
#include <string>
#include <vector>   
#include <fstream>  
#include <algorithm> // Added for std::sort

using namespace std;

enum Difficulty { HARD = 0, MEDIUM = 1, EASY = 2 };

void print(string str) {
    cout << str << endl;
}

class FlashCard {
private:
    string question;
    string answer;
    Difficulty difficulty;

public:
    FlashCard() { difficulty = MEDIUM; }  

    void setQuestion() {
        cout << "Add a question: ";
        getline(cin, question);
    }

    void setAnswer() {
        cout << "Add the answer: ";
        getline(cin, answer);
    }

    void loadCardDetails(string q, string a, Difficulty d) {
        question = q;
        answer = a;
        difficulty = d;
    }

    string getQuestion()       { return question; }
    string getAnswer()         { return answer; }
    Difficulty getDifficulty() const { return difficulty; }

    void setDifficulty(Difficulty d) { difficulty = d; }

    string getDifficultyLabel() {
        if (difficulty == HARD)   return "Hard";
        if (difficulty == MEDIUM) return "Medium";
        return "Easy";
    }
};

class UserProgress {
private:
    FlashCard *card;  // Aggregation

public:
    UserProgress(FlashCard *c) { card = c; }

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
    Deck myDeck;
    string filename = "flashcards.txt";

    // Load saved cards automatically at startup
    FileManager::loadFromFile(myDeck, filename);

    int choice = 0;

    do {
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

        cout << "Enter your choice (number): ";
        cin >> choice;
        cin.ignore(); // Clears the input buffer so getline() doesn't skip
        cout << endl;

        if (choice == 1) {
            cout << "--- Creating a Card ---" << endl;
            myDeck.addNewCard();
        }
        else if (choice == 2) {
            myDeck.removeCard();
        }
        else if (choice == 3) {
            vector<FlashCard>& cards = myDeck.getCards();
            
            if (cards.empty()) {
                print("The deck is empty! Add some cards first.");
            } else {
                myDeck.sortByDifficulty();

                for (size_t i = 0; i < cards.size(); i++) {
                    StudySession session(&cards[i]);
                    session.startSession(i + 1, cards.size());
                }
            }
        }
        else if (choice == 4) {
            vector<FlashCard>& cards = myDeck.getCards();
            
            if (cards.empty()) {
                print("The deck is empty! No progress to show.");
            } else {
                for (size_t i = 0; i < cards.size(); i++) {
                    UserProgress track(&cards[i]);
                    track.progress();
                }
                cout << "Press ENTER to return to the menu...";
                cin.ignore();
            }
        }
        else if (choice == 5) {
            cout << "Saving your deck..." << endl;
            FileManager::saveToFile(myDeck, filename);
        }
        else if (choice == 6) {
            print("Goodbye!");
            return 0;
        }
        else {
            print("Invalid choice. Please select 1-6.");
        }

    } while (true);

    return 0;
}
