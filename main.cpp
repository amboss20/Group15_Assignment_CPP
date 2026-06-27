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


class File{

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
        
        // Read chunks of 3 lines until the end of the file
        while (getline(inFile, qLines) && getline(inFile, aLines) && getline(inFile, scoreLines)) {
            // convert the score from a string to an integer
            int savedScore = stoi(scoreLines); 

            FlashCard loadedCard;
            loadedCard.loadCardDetails(qLines, aLines, savedScore);
            
            
            deck.getCards().push_back(loadedCard);
        }

        inFile.close();
        print("Deck loaded successfully from " + filename);
    }
};


};


int main() {

    return 0;
}