FLASHCARD APP

CONSOLE VERSION
Compile:
  g++ flashcards.cpp -o flashcards_console -std=c++17

Run:
  ./flashcards_console

Menu:
  1. Add FlashCard
  2. Remove FlashCard
  3. Review Flashcards
  4. View Progress
  5. Save Deck
  6. Exit


GUI VERSION
Open Git Bash in the project folder, then:

Compile:
  g++ flashcards_gui.cpp -o flashcards $(fltk-config --cxxflags --ldflags) -std=c++17

Run:
  ./flashcards

Screens:
  1. Add FlashCard     - enter question and answer, click Save Card
  2. Remove FlashCard  - select a card from the list, click Remove Selected
  3. Review Flashcards - reveal answer, rate as Hard / Medium / Easy
  4. View Progress     - see all cards and their difficulty
  5. Save Deck         - saves your cards
  6. Exit              - saves and closes the app

NOTE: Always exit using the Exit button. Closing with X does not save.
