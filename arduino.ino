#include <LiquidCrystal_I2C.h>
#include <string.h>

class PokemonMatch 
{
  public:
	PokemonMatch()
  {
	mbestOfOne = false;
	mScore = 0;
	mOPScore = 0;
	mTie = 0;
  }
	void isBestOfOne(bool is_it){mbestOfOne = is_it;}
	void incrementScore()
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
	void incrementOpScore()
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
	void decrementScore()
  {
	if (mScore == 0)
	{mScore = 0;}
	else
	{mScore--;}
  }
	void decrementOpScore()
  {
	if (mOPScore == 0)
	{mOPScore = 0;}
	else
	{mOPScore--;}
  }

	void addTie()
  {
	if (mTie >= 1)
	{mTie = 1;}
	else
	{mTie++;}
  }
	void removeTie()
  {
	if (mTie <= 0)
	{mTie = 0;}
	else
	{mTie--;}
  }
	String getScore()
  {
	String myScore = String(mScore);
	String OpScore = String(mOPScore);
	String tie = String(mTie);
	return myScore + "-" + OpScore + "-" + tie;
  }
  void dump()
  {
    mScore = 0;
    mOPScore = 0;
    mTie = 0;
  }

  private:
	bool mbestOfOne;
	int mScore;
	int mOPScore;
	int mTie;
};

struct Date
{
  int day;
  int month;
  int year;
};

struct Tournament
{
  PokemonMatch array[16] = {};
  int size = 0;
  struct Date date;
};

struct TimerState 
{
  unsigned long totalSeconds = 2100;
  unsigned long lastMillis = 0;
  bool running = false;
};


//GLOBAL VARIABLES
TimerState pkmTimer;
Tournament global;
LiquidCrystal_I2C lcd(0x27, 16, 2); 
class PokemonMatch;
const int buttonBack = 8;
const int buttonAdd = 10;
const int buttonMinus = 7;
const int PULSES_PER_STEP = 2; 

void updateTimer() {
  if (pkmTimer.running) {
    unsigned long currentMillis = millis();
    
    // Check if 1 second has passed
    if (currentMillis - pkmTimer.lastMillis >= 1000) {
      
      // ONLY decrement if there is time left
      if (pkmTimer.totalSeconds > 0) {
        pkmTimer.totalSeconds--; 
      } else {
        // Stop the timer when it hits 0
        pkmTimer.running = false;
        pkmTimer.totalSeconds = 0;
      }
      
      pkmTimer.lastMillis += 1000;
    }
  }
}

int READKNOB(String direction) 
{
  static int lastClk = HIGH;
  static int lastButton = HIGH;
  static int pulseCount = 0;
  static unsigned long lastRotationTime = 0;
  static unsigned long lastPressTime = 0;
  const int DEBOUNCE = 5;

  if (direction == "UPDATE") {
    int clkVal = digitalRead(2);
    if (clkVal == LOW && lastClk == HIGH && (millis() - lastRotationTime > DEBOUNCE)) {
      lastRotationTime = millis();
      if (digitalRead(3) == HIGH) {
        pulseCount++;
      } else {
        pulseCount--;
      }
    }
    lastClk = clkVal;
    return LOW;
    }

  if (direction == "RIGHT") {
    if (pulseCount >= PULSES_PER_STEP) {
      pulseCount = 0;
      return HIGH;
    }
    return LOW;
  }

  if (direction == "LEFT") {
    if (pulseCount <= -PULSES_PER_STEP) {
      pulseCount = 0;
      return HIGH;
    }
    return LOW;
  }

  if (direction == "PRESS") {
    int btnVal = digitalRead(6);
    if (btnVal == LOW && lastButton == HIGH && millis() - lastPressTime > 300) {
      lastButton = btnVal;
      lastPressTime = millis();
      return HIGH;
    }
    lastButton = btnVal;
    return LOW;
  }

  return LOW;
}

void setup() 
{
  lcd.init();
  lcd.backlight();
  pinMode(buttonBack, INPUT_PULLUP);
  pinMode(buttonAdd, INPUT);
  pinMode(2, INPUT_PULLUP); // CLK
  pinMode(3, INPUT_PULLUP); // DT
  pinMode(6, INPUT_PULLUP); // Click/Switch

}

  void loop() 
{

  READKNOB("UPDATE");
  updateTimer();
  static unsigned long lastMove = 0;
  static const String games[3] = {"Pokemon", "Magic", "Riftbound"};
  static int index = 0;
  static int initialization = 0;
  if (initialization == 0){updateScreen(index,games);initialization++;}
  if (millis() - lastMove > 250) {
    
    int right = READKNOB("RIGHT");
    int left = READKNOB("LEFT");  // read BOTH before acting on either
    
    if (right == HIGH) {
      index++;
      if (index > 2) index = 0;
      updateScreen(index, games);
      lastMove = millis();
    }
    else if (left == HIGH) {  // use else if so only one fires per check
      index--;
      if (index < 0) index = 2;
      updateScreen(index, games);
      lastMove = millis();
    }
  }

  if (READKNOB("PRESS") == HIGH) {
    selectGame(index);
    updateScreen(index, games);
  }
}

void updateScreen(int &index, const String* arr)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Choose Game!: ");
  lcd.print(index);
  lcd.setCursor(0,1);
  lcd.print("> ");
  lcd.print(arr[index]);
}

void MagicGame(String gameType)
{
  bool inSubMenu = true;
  bool needsRefresh = true;
  int lifeTotalStart = 40;
  int commandTax = 0;
  if (gameType == "Commander"){lifeTotalStart = 40;}
  else{lifeTotalStart = 20;}
  int opLifeTotal = 20;
  int opPoison = 0;
  int poison = 0;
  int pointerLine = 0;
  String status[3] = {"Command Tax: +", "Poison: ", "Radiation: "};
  int statusIndex = 0;
  while (inSubMenu)
  {
    READKNOB("UPDATE");
    if (needsRefresh)
    {
      lcd.clear();
      if(gameType == "Commander")
      {
        lcd.setCursor(0,0);
        lcd.print("Life Total: ");
        lcd.print(lifeTotalStart);
        lcd.setCursor(0,1);
        lcd.print("CommandTax: +");
        lcd.print(commandTax);
      }else{
        lcd.setCursor(0,0);
        lcd.print("Yourself: ");
        lcd.print(lifeTotalStart);
        lcd.setCursor(0,1);
        lcd.print("Opponent: ");
        lcd.print(opLifeTotal);
      }
      lcd.setCursor(15,pointerLine);
      lcd.print("<");
      needsRefresh = false;
    }

    if (digitalRead(buttonBack) == LOW)
    {
      while(digitalRead(buttonBack) == LOW);
      inSubMenu = false;
    }
    if (READKNOB("LEFT") == HIGH || READKNOB("RIGHT") == HIGH)
    {
      if (pointerLine == 1){pointerLine = 0;}else{pointerLine = 1;}
      needsRefresh = true;
    }
    if (digitalRead(buttonMinus) == LOW)
    {
      while(digitalRead(buttonMinus) == LOW);
      switch(pointerLine)
      {
        case 0:
          lifeTotalStart--;
          break;
        case 1:
          if (gameType == "Commander")
          {
            commandTax -= 2;
            if(commandTax < 0)
            {
              commandTax = 0;
            }
          }
          else
          {
            opLifeTotal--;
          }
          break;

      }
    needsRefresh = true;
    }
    if (digitalRead(buttonAdd) == LOW)
    {
      while(digitalRead(buttonAdd) == LOW);
      switch(pointerLine)
      {
        case 0:
          lifeTotalStart++;
          break;
        case 1:
          if (gameType == "Commander")
          {
            commandTax += 2;
          }
          else
          {
            opLifeTotal++;
          }
          break;
      }
      needsRefresh = true;
    }
  }
}

void MagicSubMenu(bool &inSubMenu)
{
  int jindex = 0;
  bool needsRefresh = true;
  const String magic[2] = {"Commander" , "Standard"};
  while (inSubMenu)
  {
  READKNOB("UPDATE");
  if (needsRefresh) { // Only update if something changed
    lcd.clear();
    lcd.print("Selected: ");
    lcd.print("Magic");
    lcd.setCursor(0,1);
    lcd.print("> ");
    lcd.print(magic[jindex]);
    needsRefresh = false;
  };

 if (READKNOB("RIGHT") == HIGH) {
    jindex++;
    if (jindex > 1) {jindex = 0;}
    needsRefresh = true; // Trigger refresh

  }

  
  if (READKNOB("LEFT") == HIGH) {
    jindex--;
    if (jindex < 0) {jindex = 1;}
    needsRefresh = true; // Trigger refresh

  }

  if (digitalRead(buttonBack) == LOW) {
    inSubMenu = false;
    needsRefresh = true; // Prepare for return to main menu
    while(digitalRead(buttonBack) == LOW); // Wait for button release
  }

  // 4. Selection logic
  if (READKNOB("PRESS") == HIGH)
  {
    while (READKNOB("PRESS") == HIGH);
    switch (jindex)
    {
      case 0:
        MagicGame("Commander");
        break;
      case 1:
        MagicGame("Standard");
        break;
    }
    needsRefresh = true;
  }
  }
}

void isTurnZeroTriggered(bool &triggered, bool& needsRefresh)
{
  lcd.clear();
  lcd.print("END OF TIME!");
  triggered = true;
  pkmTimer.totalSeconds = 600;
  delay(1000); 
  needsRefresh = true;
}

void PokemonTimer(bool &inSubMenu,PokemonMatch &current )
{
  while(digitalRead(6) == LOW); // Wait for release
  bool needsRefresh = true;
  static int lastDisplayedSeconds = -1;
  static unsigned long minusPressStart = 0;
  static bool minusIsBeingHeld = false;
  static bool turnZeroTriggered = false;
  int turnNumber = 0;
  
  while (inSubMenu)
  {
    updateTimer();
    READKNOB("UPDATE"); 

    // --- 1. MINUS BUTTON LOGIC (Handles Turn 0) ---
    if (digitalRead(buttonMinus) == LOW) {
      if (!minusIsBeingHeld) {
        minusPressStart = millis();
        minusIsBeingHeld = true;
      } else if (millis() - minusPressStart > 1000) { 
        if (!turnZeroTriggered) {
          isTurnZeroTriggered(turnZeroTriggered, needsRefresh);
        }
      }
    else if(turnZeroTriggered)
    {
      if (turnNumber != 0)
      turnNumber--;
      while(digitalRead(buttonMinus) == LOW);
      needsRefresh = true;
    } 
    } else {
      minusIsBeingHeld = false;
    } 

    if (pkmTimer.totalSeconds != lastDisplayedSeconds) {
        needsRefresh = true;
        lastDisplayedSeconds = pkmTimer.totalSeconds; 
    }

    if (needsRefresh)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Time: ");
      int minutes = pkmTimer.totalSeconds / 60;
      int seconds = pkmTimer.totalSeconds % 60;
      if (minutes < 10) lcd.print("0");
      lcd.print(minutes);
      lcd.print(":");
      if (seconds < 10) lcd.print("0");
      lcd.print(seconds);
      
      lcd.setCursor(0, 1);
      if (turnZeroTriggered)
      {
        lcd.print("Turn ");
        lcd.print(turnNumber);
        needsRefresh = false;
      }
      else
      {
        lcd.print(pkmTimer.running ? "Status: RUNNING" : "Status: STOPPED");
        needsRefresh = false;
      }
    }
    if (pkmTimer.totalSeconds == 0)
    {isTurnZeroTriggered(turnZeroTriggered, needsRefresh);}
    if (pkmTimer.totalSeconds == 0 || (turnZeroTriggered && turnNumber == 3))
    {
      PokemonCurrentScore(inSubMenu, current);
      turnZeroTriggered = false;
      pkmTimer.totalSeconds = 2100;
      pkmTimer.running = false;
      needsRefresh = true;
    }
    // --- 3. OTHER BUTTONS (Also outside the button block) ---
    
    // Toggle Run/Stop on Click
    if (READKNOB("PRESS") == HIGH)
    {
      pkmTimer.running = !pkmTimer.running;
      pkmTimer.lastMillis = millis(); 
      needsRefresh = true;
    }

    // Reset timer with Button Add
    if (digitalRead(buttonAdd) == LOW)
    {
      if (turnZeroTriggered)
      {
        while(digitalRead(buttonAdd) == LOW);
        delay(50);
        if (turnNumber != 3)
          turnNumber++;
        needsRefresh = true;
      }
      else
      {
      pkmTimer.totalSeconds = 2100;
      pkmTimer.running = false;
      needsRefresh = true;
      while(digitalRead(buttonAdd) == LOW);
      }
    }
    
    // Back button
    if (digitalRead(buttonBack) == LOW)
    {
      if (turnZeroTriggered)
      {
        turnZeroTriggered = false;
        while(digitalRead(buttonBack) == LOW);
      }
      else
      {
      inSubMenu = false;
      while(digitalRead(buttonBack) == LOW);
      }
    }
  } // <--- End of while(inSubMenu)
}

void PokemonCurrentScore(bool &inSubMenu , PokemonMatch &current)
{
  int cursorIndex = 0;
  bool needsRefresh = true;
  int scores[3] = {1, 3, 5};
  while (inSubMenu)
  {
  updateTimer();
  READKNOB("UPDATE");
  if (needsRefresh) {
    String curr = current.getScore();
    lcd.setCursor(0,0);
    lcd.print("Selected: PKMTCG");
    lcd.setCursor(0,1);
    
    // Efficiently draw the selector line
    lcd.print(cursorIndex == 0 ? ">" : " ");
    lcd.print(curr.charAt(0));
    lcd.print(cursorIndex == 1 ? ">" : "-");
    lcd.print(curr.charAt(2));
    lcd.print(cursorIndex == 2 ? ">" : "-");
    lcd.print(curr.charAt(4));
    lcd.print("           "); 
    needsRefresh = false;
  }
  // Check Encoder
  if (READKNOB("RIGHT") == HIGH) {
    cursorIndex = (cursorIndex + 1) % 3;
    needsRefresh = true;
  }
  if (READKNOB("LEFT") == HIGH) {
    cursorIndex = (cursorIndex - 1 + 3) % 3;
    needsRefresh = true;
  }
  if (digitalRead(buttonBack) == LOW) {
  inSubMenu = false;
  while(digitalRead(buttonBack) == LOW);
  continue;
  }

  // Check Buttons
  if (digitalRead(buttonAdd) == LOW) {
    if (scores[cursorIndex] == 1) current.incrementScore();
    if (scores[cursorIndex] == 3) current.incrementOpScore();
    if (scores[cursorIndex] == 5) current.addTie();
    needsRefresh = true;
    while(digitalRead(buttonAdd) == LOW); // Wait for release instead of delay
  }

  if (digitalRead(buttonMinus) == LOW) {
    if (scores[cursorIndex] == 1) current.decrementScore();
    if (scores[cursorIndex] == 3) current.decrementOpScore();
    if (scores[cursorIndex] == 5) current.removeTie();
    needsRefresh = true;
    while(digitalRead(buttonMinus) == LOW); // Wait for release
  }
  }
}

void PokemonSaveMatch(PokemonMatch &current)
{
  while (READKNOB("PRESS") == HIGH);
  if (current.getScore() == "0-0-0")
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Match");
    lcd.setCursor(0,1);
    lcd.print("is Empty");
    delay(500);
    return;
  }
  else
  {
    if (global.size == 16)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tournament Full!");
    delay(500);
    return;
  }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Match Saved!");
    delay(500);
  }
  PokemonMatch temp = current;
  global.array[global.size] = temp;
  global.size++;
  current.dump();
}

void deleteMatch(int &kindex)
{
  while(READKNOB("PRESS") == HIGH);
  bool inSubMenu = true;
  bool needsRefresh = true;
  bool yesOrNo = true;
  while (inSubMenu)
  {
    READKNOB("UPDATE");
    if (needsRefresh)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Delete Round ");
      lcd.print(kindex+1);
      lcd.print("?");
      lcd.setCursor(0,1);
      if (yesOrNo)
      {
        lcd.print(">Yes  No");
      }
      else
      {
        lcd.print(" Yes >No");
      }
      needsRefresh =false;
    }
    if (READKNOB("RIGHT") == HIGH || READKNOB("LEFT") == HIGH)
    {
      if (yesOrNo){yesOrNo = false;}
      else {yesOrNo = true;}
      needsRefresh = true;
    }

    if (READKNOB("PRESS") == HIGH && yesOrNo)
    {
      while(READKNOB("PRESS") == HIGH);
      for (int i = kindex; i < global.size - 1;i++)
      {
        global.array[i] = global.array[i + 1];

      }
      global.size -= 1;
      kindex--;
      inSubMenu = false;

    }
    if (digitalRead(buttonBack) == LOW || (READKNOB("PRESS") == HIGH && !yesOrNo))
    {
      while (digitalRead(buttonBack) == LOW);
      while (READKNOB("PRESS") == HIGH);
      inSubMenu = false;
      return;
    }
  }
}

void PokemonViewHistory(bool &inSubMenu)
{
  while (READKNOB("PRESS") == HIGH);
  bool isPressed = (READKNOB("PRESS") == HIGH);
  bool needsRefresh = true;
  int kindex = 0;
  while (inSubMenu)
  {
    updateTimer();
    READKNOB("UPDATE");

    if (needsRefresh)
    {
      if (global.size == 0)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Tournament");
        lcd.setCursor(0,1);
        lcd.print("is Empty!");
        delay(500);
        inSubMenu = false;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Tournament: ");
        lcd.setCursor(0, 1);
        lcd.print("Round ");
        lcd.print(kindex + 1);
        lcd.print(": " + global.array[kindex].getScore());
      }
      needsRefresh = false;
    }
    if (READKNOB("RIGHT") == HIGH)
    {
      kindex++;
      if (kindex > global.size - 1) {kindex = global.size -1;}
      needsRefresh = true;
    }
    if (READKNOB("LEFT") == HIGH)
    {
      kindex--;
      if (kindex < 0) {kindex = 0;}
      needsRefresh =  true;
    }
    if (digitalRead(buttonBack) == LOW)
    {
      inSubMenu = false;
      while (digitalRead(buttonBack) == LOW);
    }
    if (READKNOB("PRESS") == HIGH)
    {
      deleteMatch(kindex);
      needsRefresh = true;
    }

  }
  
}

void PokemonSubMenu(bool &inSubMenu)
{
  while (READKNOB("PRESS") == HIGH);
  
  static PokemonMatch current;
  bool needsRefresh = true;
  static const String array[5] = {"Current Score", "Save Match", "View Tournament","Timer", "View History"};
  int kindex = 0;

  while (inSubMenu)
  {
    updateTimer();
    READKNOB("UPDATE");
    bool isPressed = (READKNOB("PRESS") == HIGH);
    bool inSubSubMenu = true;
    if (needsRefresh)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Selected: PKMTCG");
      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.print(array[kindex]);
      needsRefresh = false;
    }
    if (isPressed)
    {
      switch (kindex)
      {
        while(READKNOB("PRESS") == HIGH);
        case 0:
          PokemonCurrentScore(inSubSubMenu, current);
          needsRefresh = true;
          break;
        case 1:
          PokemonSaveMatch(current);
          needsRefresh = true;
          break;
        case 2:
          PokemonViewHistory(inSubSubMenu);
          needsRefresh = true;
          break;
        case 3:
          PokemonTimer(inSubSubMenu, current);
          needsRefresh = true;
          break;
      }
    }
    while(digitalRead(buttonBack) == LOW || READKNOB("PRESS") == HIGH || digitalRead(buttonAdd) == LOW);

    if (digitalRead(buttonBack) == LOW)
    {
      inSubMenu = false;
    }

    if (READKNOB("RIGHT") == HIGH) { kindex = (kindex + 1) % 5; needsRefresh = true; }
    if (READKNOB("LEFT")  == HIGH) { kindex = (kindex - 1 + 5) % 5; needsRefresh = true; }

    }
}

void RiftboundSubMenu(bool &inSubMenu)
{
  static int player = 0;
  static int myPoints = 0;
  static int opPoints = 0;
  bool needsUpdate = true;
  while (inSubMenu)
  {
  READKNOB("UPDATE");
  if (needsUpdate){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Yourself: ");
  lcd.print(myPoints);
  lcd.setCursor(0,1);
  lcd.print("Opponent: ");
  lcd.print(opPoints);
  lcd.setCursor(15,player);
  lcd.print("<");
  needsUpdate = false;
  }
  if (READKNOB("RIGHT") == HIGH || READKNOB("LEFT") == HIGH)
  {
    player++;
    if (player == 2){player = 0;}
    if (player == 0)
    {
      needsUpdate = true;
    }
    if (player == 1)
    {
      needsUpdate = true;
    }
    delay(200);
  }
  if (digitalRead(buttonAdd) == LOW)
  {
    switch (player)
    {
      case 0:
        myPoints++;
        if (myPoints == 9)
        {myPoints = 8;}
        break;
      case 1:
        opPoints++;
        if (opPoints == 9)
        {opPoints = 8;}
        break;
    }
    while(digitalRead(buttonAdd) == LOW); 
    needsUpdate = true;
  }
  if (digitalRead(buttonMinus) == LOW)
  {
    switch(player)
    {
      case 0:
        myPoints--;
        if (myPoints == -1)
        {myPoints = 0;}
        break;
      case 1:
        opPoints--;
        if (opPoints == -1)
        {opPoints = 0;}
        break;
    }
    while(digitalRead(buttonMinus) == LOW);
     needsUpdate = true;
  }
  if (digitalRead(buttonBack) == LOW) {
    inSubMenu = false;
    while(digitalRead(buttonBack) == LOW); 
    delay(50);
    return;
  }
  }
}

void selectGame(int &index)
{
  while(READKNOB("PRESS") == HIGH); 
  bool inSubMenu = true;
  lcd.clear();
  if (index == 1)
  {
    MagicSubMenu(inSubMenu);
  }
  if(index == 0)
  {
    PokemonSubMenu(inSubMenu);
  }
  if(index == 2)
  {
    RiftboundSubMenu(inSubMenu);
  }
}
