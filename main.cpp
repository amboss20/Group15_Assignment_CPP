#include <iostream>
#include <string.h>
using namespace std;

class FlashCard {
    private:
    string question;
    string answer;
    int score;

    public:
    FlashCard () { score=0; }

    void setQuestion () {
        cout << "Add a question: ";
        getline (cin, question);
    }

    void setAnswer () {
        cout << "Add the answer: ";
        getline (cin, answer);
    }

    string getQuestion () { return question; }

    string getAnswer () { return answer; }

    int increaseScore () {
        score++;
        return score;
    }

    int decreaseScore () {
        score--;
        return score;
    }
};

int main() {


    return 0;
}