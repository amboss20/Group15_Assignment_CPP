#include <iostream>
#include <string.h>
using namespace std;

class FlashCard {
    private:
    string question;
    string answer;
    static int score;

    public:
    FlashCard () { }

    void setQuestion () {
        cout << "Add a question: ";
        getline (cin, question);
    }

    void setAnswer () {
        cout << "Add the answer: ";
        getline (cin, answer);
    }

    void prompt () {
        string userAns;
        cout << "Question: ";
        cout << question << endl;

        cout << "Enter your answer: ";
        getline (cin, userAns);
        cout<<endl;

        if (userAns == answer) {
            increaseScore();
        }

        else {
            decreaseScore();
            if (score<0)  {score=0;}
        }

        cout << "Correct answer: ";
        cout << answer << endl;
        cout << "Score: ";
        cout << score << endl;
    }

    string getQuestion () {return question;}

    string getAnswer () {return answer;}

    int increaseScore () {
        score++;
        return score;
    }

    int decreaseScore () {
        score--;
        return score;
    }
};

int FlashCard::score = 0;

int main() {
    // sample of class FlashCard
    FlashCard c1;
    cout << endl;
    c1.setQuestion();
    c1.setAnswer();
    cout << endl;

    c1.prompt();
    return 0;
}