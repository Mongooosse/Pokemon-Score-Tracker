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

byte BOLD[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

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
	if (mTie == 1)
	{mTie = 1;}
	else
	{mTie++;}
}

void PokemonMatch::removeTie()
{
	if (mTie == 0)
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

int jindex = 0;

void MagicSubMenu(bool &inSubMenu)
{
      int lastSelectState = digitalRead(buttonSelect);
      const String magic[2] = {"Commander", "Standard"};
      lcd.setCursor(0,0);
      lcd.print("Selected: ");
      lcd.print(games[index]);
      lcd.setCursor(0,1);
      lcd.print("> ");
      lcd.print(magic[jindex]);
      lcd.print("                        ");
      if (digitalRead(buttonAdd) == HIGH)
      {
        jindex++;
        if (jindex > 1) {jindex = 0;}
        delay(200);
      }
      if (digitalRead(buttonMinus) == HIGH){inSubMenu = false;}
      if (digitalRead(buttonSelect) == HIGH && lastSelectState == LOW)
      {
        inSubMenu = false;
        lcd.clear();
        lcd.print("INPUT MAGIC HERE");
        delay(1000);
      }
    lastSelectState = digitalRead(buttonSelect);
  }

void PokemonSubMenu(bool &inSubMenu)
{
  int lastSelectState = digitalRead(buttonSelect);
      pokemonMatch current;
      lcd.setCursor(0,0);
      lcd.print("Selected: ");
      lcd.print(games[index]);
      lcd.setCursor(0,1);
      lcd.print("> ");
      lcd.print(current.getScore());
      lcd.print("                        ");
      if (digitalRead(buttonAdd) == HIGH)
      {
        current.incrementScore
        delay(200);
      }
      if (digitalRead(buttonMinus) == HIGH){inSubMenu = false;}
      if (digitalRead(buttonSelect) == HIGH && lastSelectState == LOW)
      {
        inSubMenu = false;
        lcd.clear();
        lcd.print("INPUT MAGIC HERE");
        delay(1000);
      }
    lastSelectState = digitalRead(buttonSelect);
}

void selectGame()
{
  bool inSubMenu = true;
  lcd.clear();
  if (index == 1)
  {
    while (inSubMenu)
    {MagicSubMenu(inSubMenu);}
  }
  else if(index == 0)
  {
    while (inSubMenu)
 
  }
}