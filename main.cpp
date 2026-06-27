#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

using namespace std;

void print(string str) {
    cout << str << endl;
}

class FlashCard {
private:
    string question;
    string answer;
    int score;

public:
    FlashCard() { score = 0; }

    void setQuestion() {
        cout << "Add a question: ";
        getline(cin, question);
    }

    void setAnswer() {
        cout << "Add the answer: ";
        getline(cin, answer);
    }

    // HELPER ADDITION: This lets the FileManager load data from a file directly
    void loadCardDetails(string q, string a, int s) {
        question = q;
        answer = a;
        score = s;
    }

    string getQuestion() { return question; }
    string getAnswer() { return answer; }
    int getScore() { return score; }

    int increaseScore() {
        score++;
        return score;
    }

    int decreaseScore() {
        score--;
        return score;
    }
};

class UserProgress {
private:
    FlashCard *card;

public:
    UserProgress(FlashCard *c) { card = c; }

    void progress() {
        cout << "User Progress:" << endl;
        cout << "Question: " << card->getQuestion() << endl;
        cout << "Point: " << card->getScore() << endl;
        cout << endl;
    }
};

class Deck{

private:
vector<FlashCard> cards;

public:

void addNewCard(){
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

    
    void playQuiz() {
        if (cards.empty()) {
            print("The deck is empty!");
            return;
        }

        string userAnswer;
        for (size_t i = 0; i < cards.size(); i++) {
            cout << "\nQuestion: " << cards[i].getQuestion() << endl;
            cout << "Your Answer: ";
            getline(cin, userAnswer);

            if (userAnswer == cards[i].getAnswer()) {
                print("Correct!");
                cards[i].increaseScore();
            } else {
                print("Wrong! The right answer was: " + cards[i].getAnswer());
                cards[i].decreaseScore();
            }

            UserProgress tracker(&cards[i]);
            tracker.progress();
        }
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
            outFile << cards[i].getScore() << endl;
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

        string qLines, aLines, scoreLines;
        while (getline(inFile, qLines) && getline(inFile, aLines) && getline(inFile, scoreLines)) {
            int savedScore = stoi(scoreLines); // Convert text score to integer

            FlashCard loadedCard;
            loadedCard.loadCardDetails(qLines, aLines, savedScore);
            deck.getCards().push_back(loadedCard);
        }

        inFile.close();
        print("Deck loaded successfully from " + filename);
    }
};

int main() {
    Deck myDeck;
    string filename = "flashcards.txt";

    // 1. Load saved cards at startup
    FileManager::loadFromFile(myDeck, filename);

    int choice = 0;

    // 2. Menu Loop
    while (choice != 3) {
        cout << "\n=========================" << endl;
        cout << "    FLASHCARD PROGRAM    " << endl;
        cout << "=========================" << endl;
        cout << "1. Add a New Flashcard" << endl;
        cout << "2. Play the Quiz" << endl;
        cout << "3. Save and Exit" << endl;
        cout << "Enter your choice (1-3): ";
        cin >> choice;

        cin.ignore(); // Clears the Enter key buffer so getline() works safely

        switch (choice) {
            case 1:
                cout << "\n--- Creating a Card ---" << endl;
                myDeck.addNewCard();
                break;

            case 2:
                myDeck.playQuiz();
                break;

            case 3:
                cout << "\nSaving your deck..." << endl;
                FileManager::saveToFile(myDeck, filename);
                cout << "Goodbye! Good luck with your assignment." << endl;
                break;

            default:
                cout << "Invalid selection. Please type 1, 2, or 3." << endl;
                break;
        }
    }

    return 0;
}