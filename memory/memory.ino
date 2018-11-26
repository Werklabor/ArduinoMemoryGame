/* Initialize variables */
int cardTypeSize = 8;
int cardSize = cardTypeSize * 2;

int FACE_DOWN = 0;
int FACE_UP_TEMPORARILY = 1;
int FACE_UP_PERMANENTLY = 2;

boolean initialized = false;

int currentState = -2; // -1 = Welcome Screen;

char cardTypes[8] = {
  '@', '*', '+', '/', '%', '&', '!', '^'
};

char cards[16];

int cardsState[16] = {
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0
};

//Change Button values to match the displayed card map
int buttonToCardMap[] = {15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0};





void setup(){
  IBridge_init();
  IBridge_LCD5110_clear();
  initializeCards();
  Serial.begin(9600);
  while (! Serial); // Wait untilSerial is ready - Leonardo
}

void initializeCards(){
  for(int i = 0; i < cardTypeSize; i++){
    cards[i] = cardTypes[i];
    cards[cardTypeSize + i] = cardTypes[i];
  }
}

void reset(int x, int y){
  IBridge_LCD5110_clear();
  IBridge_LCD5110_set_XY(x, y);
}


int countTemporarilyFaceUpCards(){
  int counter = 0;
  for(int i = 0; i < cardSize; i++){
    if(cardsState[i] == FACE_UP_TEMPORARILY){
      counter += 1;
    }
  }
  return counter;
}

boolean areTemporarilyFaceUpCardsEqual(){
  char cardValue = '_';
  for(int i = 0; i < cardSize; i++){          // loop through all cards
    if(cardsState[i] == FACE_UP_TEMPORARILY){ // filter out any that are not temporarily face up
      if(cardValue == '_'){                     // if no temporarily face up cards are found yet
        cardValue = cards[i];                 // … save the current card's value
      }else{
        return cardValue == cards[i];         // … and compare it to the next temporarily face up card
      }
    }
  }
  return false;
}

void setTemporarilyFaceUpCardsState(int state){
  for(int i = 0; i < cardSize; i++){          // loop through all cards
    if(cardsState[i] == FACE_UP_TEMPORARILY){ // filter out any that are not temporarily face up
      cardsState[i] = state;                  // … and set their card state to the given state
    }
  }
}

void flipCard(int card){
  if(cardsState[card] != FACE_UP_PERMANENTLY) {
    Serial.write(cards[card]);
    cardsState[card] = FACE_UP_TEMPORARILY;
  }
}


void randomizeCards(){
  for(int iteration = 0; iteration < 8; iteration++){
    for(int i = 0; i < cardSize; i++){
      char j = random(0, cardSize - 1);
      char temp = cards[i];
      cards[i] = cards[j];
      cards[j] = temp;
    }
  }
}

void displayCards(){
  for(int i = 0; i < cardSize; i++){

    // IBridge Display has a width of 14 char
    if(i % 4 == 0){
      IBridge_LCD5110_write_char(' ');
    }
    IBridge_LCD5110_write_char('[');
    if(cardsState[i] != FACE_DOWN){
      IBridge_LCD5110_write_char(cards[i]);
    }else{
      IBridge_LCD5110_write_char(' ');
    }
    IBridge_LCD5110_write_char(']');
    if(i % 4 == 3){
      IBridge_LCD5110_write_char(' ');
    }
  }
}
void displayWelcomeScreen(){
  // ------------------------  "              "
  IBridge_LCD5110_write_string("              ");
  IBridge_LCD5110_write_string(" M E M O R Y  ");
  IBridge_LCD5110_write_string(" -Marc & Tobi- ");
  IBridge_LCD5110_write_string("              ");
  IBridge_LCD5110_write_string(" Press a key  ");
  IBridge_LCD5110_write_string(" to start!    ");
}



void loop()
{
  unsigned char button = 0;
  unsigned char card = 0;

  while(1){
    button = IBridge_Read_Key(); //Button values range from 1-16, 0 is no button pressed

    if(!initialized){
      if(currentState != -1){
        reset(0, 0);
        displayWelcomeScreen();
        currentState = -1;
      }
      if(button != 0){
        randomSeed(millis());
        randomizeCards();
        initialized = true;
      }
    } else {
      if(button == 0){
        if(currentState != button){
          reset(0, 1);
          displayCards();
          currentState = button;
        }
      } else {
        card = buttonToCardMap[button - 1];

        if(currentState != button){
          reset(0, 1);
          flipCard(card);
          displayCards();
          currentState = button;
        }

        if(countTemporarilyFaceUpCards() >= 2){
          if(areTemporarilyFaceUpCardsEqual()){
            setTemporarilyFaceUpCardsState(FACE_UP_PERMANENTLY);
          }else{
            delay(1 * 1000);
            setTemporarilyFaceUpCardsState(FACE_DOWN);
          }
        }
      }
    }

    button = 0;
  }
}
