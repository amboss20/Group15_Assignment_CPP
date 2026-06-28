#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Group.H>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

// ─────────────────────────────────────────────
//  Colour palette  (FLTK uses 0xRRGGBB00)
// ─────────────────────────────────────────────
#define COL_BG        fl_rgb_color(245, 245, 250)   // soft off-white background
#define COL_CARD      fl_rgb_color(255, 255, 255)   // white card surface
#define COL_PRIMARY   fl_rgb_color( 79, 109, 245)   // indigo accent
#define COL_HARD      fl_rgb_color(220,  80,  80)   // red
#define COL_MEDIUM    fl_rgb_color(230, 160,  40)   // amber
#define COL_EASY      fl_rgb_color( 60, 179, 113)   // green
#define COL_DANGER    fl_rgb_color(210,  60,  60)   // remove / danger
#define COL_MUTED     fl_rgb_color(140, 140, 155)   // secondary text

// ─────────────────────────────────────────────
//  Existing domain classes (unchanged)
// ─────────────────────────────────────────────
enum Difficulty { HARD = 0, MEDIUM = 1, EASY = 2 };

class FlashCard {
private:
    string question, answer;
    Difficulty difficulty;
public:
    FlashCard() { difficulty = MEDIUM; }

    void loadCardDetails(string q, string a, Difficulty d) {
        question = q; answer = a; difficulty = d;
    }

    string getQuestion()            { return question; }
    string getAnswer()              { return answer; }
    Difficulty getDifficulty() const{ return difficulty; }
    void setDifficulty(Difficulty d){ difficulty = d; }

    string getDifficultyLabel() {
        if (difficulty == HARD)   return "Hard";
        if (difficulty == MEDIUM) return "Medium";
        return "Easy";
    }
};

class Deck {
private:
    vector<FlashCard> cards;
public:
    vector<FlashCard>& getCards() { return cards; }

    void addCard(const string& q, const string& a) {
        FlashCard c;
        c.loadCardDetails(q, a, MEDIUM);
        cards.push_back(c);
    }

    void removeCard(int index) {
        if (index >= 0 && index < (int)cards.size())
            cards.erase(cards.begin() + index);
    }

    void clearDeck() { cards.clear(); }

    void sortByDifficulty() {
        sort(cards.begin(), cards.end(), [](const FlashCard& a, const FlashCard& b) {
            return a.getDifficulty() < b.getDifficulty();
        });
    }
};

class FileManager {
public:
    static void saveToFile(Deck& deck, const string& filename) {
        ofstream out(filename);
        if (!out) return;
        for (auto& c : deck.getCards()) {
            out << c.getQuestion()  << "\n"
                << c.getAnswer()    << "\n"
                << static_cast<int>(c.getDifficulty()) << "\n";
        }
        out.close();
    }

    static void loadFromFile(Deck& deck, const string& filename) {
        ifstream in(filename);
        if (!in) return;
        deck.clearDeck();
        string q, a, d;
        while (getline(in, q) && getline(in, a) && getline(in, d)) {
            if (q.empty() || a.empty() || d.empty()) continue;
            int diffVal = 1; // default MEDIUM if unreadable
            try { diffVal = stoi(d); } catch (...) { continue; }
            if (diffVal < 0 || diffVal > 2) diffVal = 1;
            FlashCard c;
            c.loadCardDetails(q, a, static_cast<Difficulty>(diffVal));
            deck.getCards().push_back(c);
        }
    }
};

// ─────────────────────────────────────────────
//  Helper: create a styled push-button
// ─────────────────────────────────────────────
static Fl_Button* makeButton(int x, int y, int w, int h,
                              const char* label, Fl_Color col)
{
    Fl_Button* b = new Fl_Button(x, y, w, h, label);
    b->color(col);
    b->labelcolor(FL_WHITE);
    b->labelfont(FL_HELVETICA_BOLD);
    b->labelsize(14);
    b->box(FL_ROUND_UP_BOX);
    return b;
}

// ─────────────────────────────────────────────
//  FlashCardApp  – one class, all screens
// ─────────────────────────────────────────────
class FlashCardApp {
private:
    // ── Window ──────────────────────────────
    Fl_Window* window;

    // ── Screens (Fl_Group acts as a panel) ──
    Fl_Group* screenMenu;
    Fl_Group* screenAdd;
    Fl_Group* screenRemove;
    Fl_Group* screenReview;
    Fl_Group* screenProgress;

    // ── Add-card screen ─────────────────────
    Fl_Multiline_Input* addQuestion;
    Fl_Multiline_Input* addAnswer;
    Fl_Box*             addStatus;

    // ── Remove screen ───────────────────────
    Fl_Select_Browser*  removeBrowser;

    // ── Review screen ───────────────────────
    Fl_Box*             reviewCounter;
    Fl_Box*             reviewQuestion;
    Fl_Box*             reviewAnswer;
    Fl_Button*          reviewRevealBtn;
    Fl_Group*           reviewFeedbackGroup;   // hard/medium/easy buttons
    int                 reviewIndex;           // current card index

    // ── Progress screen ─────────────────────
    Fl_Scroll*          progressScroll;
    Fl_Group*           progressInner;

    // ── Data ────────────────────────────────
    Deck    deck;
    string  filename;

    // ════════════════════════════════════════
    //  Screen switching
    // ════════════════════════════════════════
    void showScreen(Fl_Group* s) {
        screenMenu->hide();
        screenAdd->hide();
        screenRemove->hide();
        screenReview->hide();
        screenProgress->hide();
        s->show();
    }

    // ════════════════════════════════════════
    //  Build helpers
    // ════════════════════════════════════════

    // ── Main menu ───────────────────────────
    void buildMenuScreen() {
        screenMenu = new Fl_Group(0, 0, 520, 600);
        screenMenu->color(COL_BG);
        screenMenu->box(FL_FLAT_BOX);

        // Title
        Fl_Box* title = new Fl_Box(0, 40, 520, 50, "FlashCard App");
        title->labelfont(FL_HELVETICA_BOLD);
        title->labelsize(28);
        title->labelcolor(COL_PRIMARY);

        Fl_Box* sub = new Fl_Box(0, 90, 520, 25, "Study smarter, one card at a time.");
        sub->labelsize(13);
        sub->labelcolor(COL_MUTED);

        // Menu buttons
        const int BW = 260, BH = 46, BX = (520 - BW) / 2;
        int by = 150;
        const int GAP = 16;

        auto* b1 = makeButton(BX, by,       BW, BH, "1.  Add FlashCard",    COL_PRIMARY); by += BH + GAP;
        auto* b2 = makeButton(BX, by,       BW, BH, "2.  Remove FlashCard", COL_DANGER);  by += BH + GAP;
        auto* b3 = makeButton(BX, by,       BW, BH, "3.  Review Flashcards",COL_PRIMARY); by += BH + GAP;
        auto* b4 = makeButton(BX, by,       BW, BH, "4.  View Progress",    COL_PRIMARY); by += BH + GAP;
        auto* b5 = makeButton(BX, by,       BW, BH, "5.  Save Deck",        COL_EASY);    by += BH + GAP;
        auto* b6 = makeButton(BX, by,       BW, BH, "6.  Exit",             COL_MUTED);

        b1->callback(cb_goAdd,      this);
        b2->callback(cb_goRemove,   this);
        b3->callback(cb_goReview,   this);
        b4->callback(cb_goProgress, this);
        b5->callback(cb_save,       this);
        b6->callback(cb_exit,       this);

        screenMenu->end();
    }

    // ── Add-card screen ──────────────────────
    void buildAddScreen() {
        screenAdd = new Fl_Group(0, 0, 520, 600);
        screenAdd->color(COL_BG);
        screenAdd->box(FL_FLAT_BOX);

        Fl_Box* title = new Fl_Box(0, 30, 520, 40, "Add a FlashCard");
        title->labelfont(FL_HELVETICA_BOLD);
        title->labelsize(22);
        title->labelcolor(COL_PRIMARY);

        // Question
        Fl_Box* ql = new Fl_Box(50, 90, 420, 22, "Question:");
        ql->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        ql->labelfont(FL_HELVETICA_BOLD);
        ql->labelsize(13);

        addQuestion = new Fl_Multiline_Input(50, 115, 420, 100);
        addQuestion->box(FL_ROUNDED_BOX);
        addQuestion->textsize(13);

        // Answer
        Fl_Box* al = new Fl_Box(50, 230, 420, 22, "Answer:");
        al->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        al->labelfont(FL_HELVETICA_BOLD);
        al->labelsize(13);

        addAnswer = new Fl_Multiline_Input(50, 255, 420, 100);
        addAnswer->box(FL_ROUNDED_BOX);
        addAnswer->textsize(13);

        // Status
        addStatus = new Fl_Box(50, 368, 420, 28, "");
        addStatus->labelsize(12);
        addStatus->labelcolor(COL_EASY);

        // Buttons
        auto* saveBtn = makeButton( 50, 410, 190, 42, "Save Card", COL_PRIMARY);
        auto* backBtn = makeButton(280, 410, 190, 42, "Back",      COL_MUTED);
        saveBtn->callback(cb_addSave, this);
        backBtn->callback(cb_back,    this);

        screenAdd->end();
        screenAdd->hide();
    }

    // ── Remove screen ───────────────────────
    void buildRemoveScreen() {
        screenRemove = new Fl_Group(0, 0, 520, 600);
        screenRemove->color(COL_BG);
        screenRemove->box(FL_FLAT_BOX);

        Fl_Box* title = new Fl_Box(0, 30, 520, 40, "Remove a FlashCard");
        title->labelfont(FL_HELVETICA_BOLD);
        title->labelsize(22);
        title->labelcolor(COL_DANGER);

        Fl_Box* hint = new Fl_Box(50, 76, 420, 22, "Select a card then press Remove.");
        hint->labelsize(12);
        hint->labelcolor(COL_MUTED);
        hint->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        removeBrowser = new Fl_Select_Browser(50, 105, 420, 370);
        removeBrowser->textsize(13);
        removeBrowser->box(FL_ROUNDED_BOX);

        auto* removeBtn = makeButton( 50, 492, 190, 42, "Remove Selected", COL_DANGER);
        auto* backBtn   = makeButton(280, 492, 190, 42, "Back",             COL_MUTED);
        removeBtn->callback(cb_doRemove, this);
        backBtn->callback(cb_back,       this);

        screenRemove->end();
        screenRemove->hide();
    }

    // ── Review screen ───────────────────────
    void buildReviewScreen() {
        screenReview = new Fl_Group(0, 0, 520, 600);
        screenReview->color(COL_BG);
        screenReview->box(FL_FLAT_BOX);

        // Counter badge at top-right
        reviewCounter = new Fl_Box(370, 18, 130, 28, "");
        reviewCounter->labelfont(FL_HELVETICA_BOLD);
        reviewCounter->labelsize(13);
        reviewCounter->labelcolor(COL_MUTED);
        reviewCounter->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

        Fl_Box* title = new Fl_Box(0, 18, 360, 28, "Review Session");
        title->labelfont(FL_HELVETICA_BOLD);
        title->labelsize(20);
        title->labelcolor(COL_PRIMARY);
        title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        title->position(20, 18);

        // Question card
        Fl_Box* ql = new Fl_Box(40, 64, 440, 22, "Question");
        ql->labelfont(FL_HELVETICA_BOLD);
        ql->labelsize(11);
        ql->labelcolor(COL_MUTED);
        ql->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        reviewQuestion = new Fl_Box(40, 88, 440, 110, "");
        reviewQuestion->box(FL_SHADOW_BOX);
        reviewQuestion->color(COL_CARD);
        reviewQuestion->labelsize(14);
        reviewQuestion->align(FL_ALIGN_WRAP | FL_ALIGN_INSIDE | FL_ALIGN_TOP_LEFT);

        // Reveal button
        reviewRevealBtn = makeButton(180, 218, 160, 40, "Reveal Answer", COL_PRIMARY);
        reviewRevealBtn->callback(cb_reveal, this);

        // Answer card (hidden until revealed)
        Fl_Box* al = new Fl_Box(40, 272, 440, 22, "Answer");
        al->labelfont(FL_HELVETICA_BOLD);
        al->labelsize(11);
        al->labelcolor(COL_MUTED);
        al->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

        reviewAnswer = new Fl_Box(40, 296, 440, 100, "");
        reviewAnswer->box(FL_SHADOW_BOX);
        reviewAnswer->color(COL_CARD);
        reviewAnswer->labelsize(14);
        reviewAnswer->align(FL_ALIGN_WRAP | FL_ALIGN_INSIDE | FL_ALIGN_TOP_LEFT);

        // Feedback group
        reviewFeedbackGroup = new Fl_Group(0, 415, 520, 120);

        Fl_Box* feedLabel = new Fl_Box(0, 415, 520, 24, "How difficult was this card?");
        feedLabel->labelfont(FL_HELVETICA_BOLD);
        feedLabel->labelsize(13);

        auto* hardBtn   = makeButton( 30, 448, 140, 44, "Hard",   COL_HARD);
        auto* medBtn    = makeButton(190, 448, 140, 44, "Medium", COL_MEDIUM);
        auto* easyBtn   = makeButton(350, 448, 140, 44, "Easy",   COL_EASY);
        hardBtn->callback(cb_hard,   this);
        medBtn->callback(cb_medium,  this);
        easyBtn->callback(cb_easy,   this);

        reviewFeedbackGroup->end();
        reviewFeedbackGroup->hide();

        // Back to menu
        auto* backBtn = makeButton(20, 548, 110, 36, "< Menu", COL_MUTED);
        backBtn->callback(cb_back, this);

        screenReview->end();
        screenReview->hide();
    }

    // ── Progress screen ─────────────────────
    void buildProgressScreen() {
        screenProgress = new Fl_Group(0, 0, 520, 600);
        screenProgress->color(COL_BG);
        screenProgress->box(FL_FLAT_BOX);

        Fl_Box* title = new Fl_Box(0, 30, 520, 40, "Your Progress");
        title->labelfont(FL_HELVETICA_BOLD);
        title->labelsize(22);
        title->labelcolor(COL_PRIMARY);

        // Scrollable area for card rows
        progressScroll = new Fl_Scroll(30, 85, 460, 460);
        progressScroll->box(FL_FLAT_BOX);
        progressScroll->color(COL_BG);
        progressInner = new Fl_Group(30, 85, 440, 460);
        progressInner->end();
        progressScroll->end();

        auto* backBtn = makeButton(195, 558, 130, 36, "Back", COL_MUTED);
        backBtn->callback(cb_back, this);

        screenProgress->end();
        screenProgress->hide();
    }

    // ════════════════════════════════════════
    //  Screen population (called each time)
    // ════════════════════════════════════════

    void populateRemove() {
        removeBrowser->clear();
        for (auto& c : deck.getCards())
            removeBrowser->add(c.getQuestion().c_str());
    }

    void populateProgress() {
        // Remove old rows
        progressInner->clear();
        progressInner->begin();

        vector<FlashCard>& cards = deck.getCards();
        int y = 85;
        for (size_t i = 0; i < cards.size(); i++) {
            // Row box
            Fl_Box* row = new Fl_Box(30, y, 440, 52);
            row->box(FL_SHADOW_BOX);
            row->color(COL_CARD);

            // Question text (truncated)
            string q = cards[i].getQuestion();
            if (q.size() > 50) q = q.substr(0, 47) + "...";

            Fl_Box* qb = new Fl_Box(42, y + 6, 300, 20, "");
            qb->copy_label(q.c_str());
            qb->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            qb->labelsize(12);

            // Difficulty badge
            Fl_Color badgeCol = (cards[i].getDifficulty() == HARD)   ? COL_HARD :
                                (cards[i].getDifficulty() == MEDIUM) ? COL_MEDIUM : COL_EASY;
            Fl_Box* badge = new Fl_Box(358, y + 14, 100, 24, "");
            badge->copy_label(cards[i].getDifficultyLabel().c_str());
            badge->box(FL_ROUND_UP_BOX);
            badge->color(badgeCol);
            badge->labelcolor(FL_WHITE);
            badge->labelfont(FL_HELVETICA_BOLD);
            badge->labelsize(12);

            y += 60;
        }

        progressInner->size(440, max(460, y - 85));
        progressInner->end();
        progressScroll->redraw();
    }

    void startReview() {
        deck.sortByDifficulty();
        reviewIndex = 0;
        loadReviewCard();
    }

    void loadReviewCard() {
        vector<FlashCard>& cards = deck.getCards();
        int total = (int)cards.size();

        // Update counter label
        string cnt = "Card " + to_string(reviewIndex + 1) + " of " + to_string(total);
        reviewCounter->copy_label(cnt.c_str());

        // Show question
        reviewQuestion->copy_label(cards[reviewIndex].getQuestion().c_str());

        // Hide answer and feedback until revealed
        reviewAnswer->copy_label("");
        reviewFeedbackGroup->hide();
        reviewRevealBtn->show();

        screenReview->redraw();
    }

    void revealAnswer() {
        vector<FlashCard>& cards = deck.getCards();
        reviewAnswer->copy_label(cards[reviewIndex].getAnswer().c_str());
        reviewRevealBtn->hide();
        reviewFeedbackGroup->show();
        screenReview->redraw();
    }

    void applyFeedback(Difficulty d) {
        deck.getCards()[reviewIndex].setDifficulty(d);
        reviewIndex++;

        if (reviewIndex >= (int)deck.getCards().size()) {
            // Session complete — save and return to menu
            FileManager::saveToFile(deck, filename);
            showScreen(screenMenu);
        } else {
            loadReviewCard();
        }
    }

    // ════════════════════════════════════════
    //  Static callbacks  →  member functions
    // ════════════════════════════════════════
    static void cb_goAdd(Fl_Widget*, void* v) {
        auto* a = static_cast<FlashCardApp*>(v);
        a->addQuestion->value("");
        a->addAnswer->value("");
        a->addStatus->label("");
        a->showScreen(a->screenAdd);
    }
    static void cb_goRemove(Fl_Widget*, void* v) {
        auto* a = static_cast<FlashCardApp*>(v);
        a->populateRemove();
        a->showScreen(a->screenRemove);
    }
    static void cb_goReview(Fl_Widget*, void* v) {
        auto* a = static_cast<FlashCardApp*>(v);
        if (a->deck.getCards().empty()) return;
        a->startReview();
        a->showScreen(a->screenReview);
    }
    static void cb_goProgress(Fl_Widget*, void* v) {
        auto* a = static_cast<FlashCardApp*>(v);
        a->populateProgress();
        a->showScreen(a->screenProgress);
    }
    static void cb_save(Fl_Widget*, void* v) {
        auto* a = static_cast<FlashCardApp*>(v);
        FileManager::saveToFile(a->deck, a->filename);
    }
    static void cb_exit(Fl_Widget*, void* v) {
        auto* a = static_cast<FlashCardApp*>(v);
        FileManager::saveToFile(a->deck, a->filename);
        a->window->hide();
    }
    static void cb_back(Fl_Widget*, void* v) {
        auto* a = static_cast<FlashCardApp*>(v);
        a->showScreen(a->screenMenu);
    }
    static void cb_addSave(Fl_Widget*, void* v) {
        auto* a = static_cast<FlashCardApp*>(v);
        string q = a->addQuestion->value();
        string ans = a->addAnswer->value();
        if (q.empty() || ans.empty()) {
            a->addStatus->label("Please fill in both fields.");
            a->addStatus->labelcolor(COL_HARD);
            return;
        }
        a->deck.addCard(q, ans);
        a->addQuestion->value("");
        a->addAnswer->value("");
        a->addStatus->label("Card saved successfully!");
        a->addStatus->labelcolor(COL_EASY);
        a->screenAdd->redraw();
    }
    static void cb_doRemove(Fl_Widget*, void* v) {
        auto* a = static_cast<FlashCardApp*>(v);
        int sel = a->removeBrowser->value();   // 1-based, 0 = none
        if (sel == 0) return;
        a->deck.removeCard(sel - 1);
        a->populateRemove();
    }
    static void cb_reveal(Fl_Widget*, void* v) {
        static_cast<FlashCardApp*>(v)->revealAnswer();
    }
    static void cb_hard(Fl_Widget*, void* v) {
        static_cast<FlashCardApp*>(v)->applyFeedback(HARD);
    }
    static void cb_medium(Fl_Widget*, void* v) {
        static_cast<FlashCardApp*>(v)->applyFeedback(MEDIUM);
    }
    static void cb_easy(Fl_Widget*, void* v) {
        static_cast<FlashCardApp*>(v)->applyFeedback(EASY);
    }

public:
    // ════════════════════════════════════════
    //  Constructor / Destructor
    // ════════════════════════════════════════
    FlashCardApp() : reviewIndex(0) {
        filename = "flashcards.txt";
        FileManager::loadFromFile(deck, filename);

        window = new Fl_Window(520, 600, "FlashCard App");
        window->color(COL_BG);

        buildMenuScreen();
        buildAddScreen();
        buildRemoveScreen();
        buildReviewScreen();
        buildProgressScreen();

        window->end();
    }

    ~FlashCardApp() {
        delete window;
    }

    void show() {
        window->show();
    }
};

// ─────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────
int main(int argc, char** argv) {
    FlashCardApp app;
    app.show();
    return Fl::run();
}