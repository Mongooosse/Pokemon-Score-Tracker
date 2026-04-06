#include <LiquidCrystal.h>
#include <string.h>
#include <Vector.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const String games[3] = {"Pokemon", "Magic", "Riftbound"};
int index = 0;

const int buttonSelect = 7;
const int buttonAdd = 8;
const int buttonMinus = 13;


void selectGame();
class PokemonMatch {
public:
	PokemonMatch();
	void isBestOfOne(bool is_it);
	void incrementScore();
	void incrementOpScore();
	void decrementScore();
	void decrementOpScore();
	void addTie();
	void removeTie();
	String getScore();

private:
	bool mbestOfOne;
	int mScore;
	int mOPScore;
	int mTie;
};

class Tournament
{
public:
	Tournament();
  Tournament(bool isBestofOne);
	bool isBestofOne() const;
	void addMatch(PokemonMatch round);
	PokemonMatch getMatch(int round) const;
	int roundSize() const;
private:
	PokemonMatch mStorage[8];
  Vector<PokemonMatch> mTournament;
	bool mBestofOne;

};

class History
{
public:
	History();
	Tournament getTournament(int i) const;
	void addTournament(Tournament tournament);
	int tournamentAmount() const;
private:
	Tournament mHistoryStorage[5];
  Vector<Tournament> mHistory;
};

PokemonMatch::PokemonMatch()
{
	mbestOfOne = false;
	mScore = 0;
	mOPScore = 0;
	mTie = 0;
};


void PokemonMatch::isBestOfOne(bool is_it) 
{mbestOfOne = is_it;}

void PokemonMatch::incrementScore()
{	
	if (!mbestOfOne)
	{
		if (mScore == 2)
		{mScore = 2;}
		else{mScore++;}
	}
	else
	{
		if (mScore == 1)
		{mScore = 1;}
		else
		{mScore++;}
	}
}

void PokemonMatch::incrementOpScore()
{
	if (!mbestOfOne)
	{
		if (mOPScore == 2)
		{mOPScore = 2;}
		else
		{mOPScore++;}
	}
	else
	{
		if (mOPScore == 1)
		{mOPScore = 1;}
		else
		{mOPScore++;}
	}
}

void PokemonMatch::decrementScore()
{
	if (mScore == 0)
	{mScore = 0;}
	else
	{mScore--;}
}

void PokemonMatch::decrementOpScore()
{
	if (mOPScore == 0)
	{mOPScore = 0;}
	else
	{mOPScore--;}
}

void PokemonMatch::addTie()
{
	if (mTie >= 1)
	{mTie = 1;}
	else
	{mTie++;}
}

void PokemonMatch::removeTie()
{
	if (mTie <= 0)
	{mTie = 0;}
	else
	{mTie--;}
}

String PokemonMatch::getScore()
{
	String myScore = String(mScore);
	String OpScore = String(mOPScore);
	String tie = String(mTie);
	return myScore + "-" + OpScore + "-" + tie;
}

Tournament::Tournament()
{
  mBestofOne = false;
  mTournament.setStorage(mStorage);
}

Tournament::Tournament(bool isBestofOne)
{mBestofOne = isBestofOne;
mTournament.setStorage(mStorage);}

bool Tournament::isBestofOne() const
{return mBestofOne;}

void Tournament::addMatch(PokemonMatch round1)
{mTournament.push_back(round1);}

PokemonMatch Tournament::getMatch(int round1) const
{
	if (round1  >= 0 && round1 < mTournament.size())
	{return mTournament[round1];}
	else{return mTournament.back();}
}

int Tournament::roundSize() const
{return mTournament.size();}

History::History()
{
  mHistory.setStorage(mHistoryStorage);
}

Tournament History::getTournament(int i) const
{
	if ( i >= 0 && i < mHistory.size())
	{return mHistory[i];}
	else {return mHistory.back();}
}
void History::addTournament(Tournament tournament)
{mHistory.push_back(tournament);}

int History::tournamentAmount() const
{return mHistory.size();}



void setup() {
  lcd.begin(16, 2);
  pinMode(buttonSelect, INPUT);
  pinMode(buttonAdd, INPUT);
  pinMode(buttonMinus, INPUT);
  pinMode(9, OUTPUT);
  analogWrite(9, 90); 
  lcd.createChar(0, BOLD);
  updateScreen();
}

void loop() {
  if (digitalRead(buttonAdd) == HIGH)
    {
      index++;
      if (index > 2){index = 0;}
      updateScreen();
      delay(250);
    }



  if (digitalRead(buttonMinus) == HIGH)
    {
      index--;
      if (index < 0){index = 2;}
      updateScreen();
      delay(250);
    }

  if (digitalRead(buttonSelect) == HIGH)
    {
      selectGame();
      delay(250);
      updateScreen();
    }
}

void updateScreen()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Choose Game!: ");
  lcd.print(index);
  lcd.setCursor(0,1);
  lcd.print("> ");
  lcd.print(games[index]);
}


void MagicSubMenu(bool &inSubMenu, int &jindex)
{
  static int lastSelectState = HIGH; 
  const String magic[2] = {"Commander", "Standard"};
  
  lcd.setCursor(0,0);
  lcd.print("Selected: ");
  lcd.print(games[index]);
  lcd.setCursor(0,1);
  lcd.print("> ");
  lcd.print(magic[jindex]);
  lcd.print("                ");

  if (digitalRead(buttonAdd) == HIGH)
  {
    jindex++;
    if (jindex > 1) {jindex = 0;}
    delay(200);
  }

  if (digitalRead(buttonMinus) == HIGH) {
    inSubMenu = false;
    while(digitalRead(buttonMinus) == HIGH); 
  }

  int selectState = digitalRead(buttonSelect);
  if (selectState == HIGH && lastSelectState == LOW)
  {
    lcd.clear();
    lcd.print("INPUT MAGIC HERE");
    delay(1000);
    inSubMenu = false;
    
    while(digitalRead(buttonSelect) == HIGH); 
  }
  lastSelectState = selectState;
}


void PokemonSubSubMenu(bool &inSubSubMenu, PokemonMatch &curr, int which)
{
  static int lastSelectStateSub = HIGH;
  
  // Display
  String s = curr.getScore();
  lcd.setCursor(0, 1);
  lcd.print("EDITING: ");
  lcd.print(s.charAt(which - 1));
  lcd.print("    "); 

  if (digitalRead(buttonAdd) == HIGH) {
    if (which == 1) curr.incrementScore();
    if (which == 3) curr.incrementOpScore();
    if (which == 5) curr.addTie();
    delay(250);
  }
  if (digitalRead(buttonMinus) == HIGH) {
    if (which == 1) curr.decrementScore();
    if (which == 3) curr.decrementOpScore();
    if (which == 5) curr.removeTie();
    delay(250);
  }

  int selectState = digitalRead(buttonSelect);
  if (selectState == HIGH && lastSelectStateSub == LOW) {
    inSubSubMenu = false; 
    while (digitalRead(buttonSelect) == HIGH); 
    delay(50);
  }
  lastSelectStateSub = selectState;
}

void PokemonSubMenu(bool &inSubMenu)
{
  while(digitalRead(buttonSelect) == HIGH);
  static int cursorIndex = 0;
  static int lastSelectState = HIGH;
  static PokemonMatch current;
  int scores[3] = {1, 3, 5};

  // Render Display
  String curr = current.getScore();
  lcd.setCursor(0,0);
  lcd.print("Selected: PKMTCG");
  lcd.setCursor(0,1);
  if (cursorIndex == 0) lcd.print(">"); else lcd.print(" ");
  lcd.print(curr.charAt(0));
  if (cursorIndex == 1) lcd.print(">"); else lcd.print("-");
  lcd.print(curr.charAt(2));
  if (cursorIndex == 2) lcd.print(">"); else lcd.print("-");
  lcd.print(curr.charAt(4));
  lcd.print("            ");

  if (digitalRead(buttonAdd) == HIGH) {
    cursorIndex++;
    if (cursorIndex > 2) cursorIndex = 0;
    delay(200);
  }
  if (digitalRead(buttonMinus) == HIGH) {
    inSubMenu = false;
    while(digitalRead(buttonMinus) == HIGH);
  }

  int selectState = digitalRead(buttonSelect);
  if (selectState == HIGH && lastSelectState == LOW) {
    bool keepEditing = true;
    lcd.clear();
    while (keepEditing) {
      PokemonSubSubMenu(keepEditing, current, scores[cursorIndex]);
      cursorIndex = 0;
    }
    lcd.clear();
    lastSelectState = HIGH; 
  } else {
    lastSelectState = selectState;
  }
}

void selectGame()
{
  int jindex = 0;
  bool inSubMenu = true;
  lcd.clear();
  if (index == 1)
  {
    while (inSubMenu)
    {
    MagicSubMenu(inSubMenu, jindex);
    jindex = 0;
    }
  }
  else if(index == 0)
  {
    while (inSubMenu)
    {PokemonSubMenu(inSubMenu);}
 
  }
}