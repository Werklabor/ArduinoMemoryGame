int sideWidth = 4;
int cardTypeSize = 8;
int cardSize = cardTypeSize * 2;

char cardTypes[8] = {
  // 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'
	'@', '*', '+', '/', '%', '&', '!', '^'
};

char cards[16];

int cardsState[16] = {
	0, 0, 0, 0,
	0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0
};
int FACE_DOWN = 0;
int FACE_UP_TEMPORARILY = 1;
int FACE_UP_PERMANENTLY = 2;

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
	for(int i = 0; i < cardSize; i++){ 					// loop through all cards
		if(cardsState[i] == FACE_UP_TEMPORARILY){ // filter out any that are not temporarily face up
			if(cardValue == '_'){     								// if no temporarily face up cards are found yet
				cardValue = cards[i];  								// … save the current card's value
			}else{
				return cardValue == cards[i]; 				// … and compare it to the next temporarily face up card
			}
		}
	}
	return false;
}
void setTemporarilyFaceUpCardsState(int state){
	for(int i = 0; i < cardSize; i++){ 					// loop through all cards
		if(cardsState[i] == FACE_UP_TEMPORARILY){ // filter out any that are not temporarily face up
			cardsState[i] = state;                  // … and set their card state to the given state
		}
	}
}
void flipCard(int card){
	if(cardsState[card] != FACE_UP_PERMANENTLY) {
		cardsState[card] = FACE_UP_TEMPORARILY;
	}
}

void initializeCards(){
  for(int i = 0; i < cardTypeSize; i++){
    cards[i] = cardTypes[i];
    cards[cardTypeSize + i] = cardTypes[i];
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

void setup(){
  IBridge_init();
  IBridge_LCD5110_clear();

  initializeCards();
}

void reset(){
  IBridge_LCD5110_clear();
  IBridge_LCD5110_set_XY(0, 0);
}

int buttonToCardMap[] = {15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0};

void displayCards(int button){
  for(int i = 0; i < cardSize; i++){
    IBridge_LCD5110_set_XY((i % sideWidth) * 2, i / sideWidth);
    // if(button == i){
		if(cardsState[i] > 0){
      IBridge_LCD5110_write_char(cards[i]);
    }else{
      IBridge_LCD5110_write_char('*');
    }
  }
}
void displayWelcomeScreen(){
	// ------------------------  "              "
	IBridge_LCD5110_write_string("              ");
	IBridge_LCD5110_write_string(" M E M O R Y  ");
	IBridge_LCD5110_write_string(" -Zeilenwerk- ");
	IBridge_LCD5110_write_string("              ");
	IBridge_LCD5110_write_string(" Press a key  ");
	IBridge_LCD5110_write_string(" to start!    ");
}

boolean initialized = false;

void loop()
{
  unsigned char button = 0;
  unsigned char card = 0;

  while(1){
    button = IBridge_Read_Key();
    reset();

		if(!initialized){
			displayWelcomeScreen();
			if(button != 0){
				randomSeed(millis());
				randomizeCards();
				initialized = true;
			}
		}else{
			if(button == 0){
	      displayCards(-1);
	    }else{
				card = buttonToCardMap[button - 1];

				flipCard(card);
				displayCards(card);

				if(countTemporarilyFaceUpCards() >= 2){
					if(areTemporarilyFaceUpCardsEqual()){
						setTemporarilyFaceUpCardsState(FACE_UP_PERMANENTLY);
					}else{
						delay(2 * 1000);
						setTemporarilyFaceUpCardsState(FACE_DOWN);
					}
				}
	    }
		}

    button = 0;
  }
}
