char *phrases[] = {
  "hallo!",
  "wazzzuuup???",
  "LOW PROBABILITY OF GHOST SEENING!",
  "WOOD WOOD WOOD",
  "HIGH PROB OF WOLFS MEETING CLOSE TO 99%",
  "ABNORMAL EMP TRACES",
  "ALSO INTENSE ALCOHOL CONTENT DETECTED",
  "ALSO A HOLE IN REALITY",
  "LOTS OF BLOOOOD",
  "AHAHAH-AHH-HHH!!!",
  "KROWISCHTA!",
  "NOTHIN HERE",
  "ELVIS LEFT FOR GOOD",
  "GOOD! GOOD! DEATH ZONE!!!",
  "THIN WALLS",
  "DEAD PEOPLE ALL AROUND",
  "THE BEAST IS NEAR!",
  "ALL HEIL TO THE BEAST!!!!",
  "ALL HEIL THE RED QUEEN",
  "KEEP YOUR BLOOD SAFE",
  "THE PLASE OF SUFFERING!",
  "BAD!",
  "NO DEATH!",
  "BAAAD!!!",
  "STRANGE POWERS!",
  "BEWARE OF TRAPS!",
  "ITS A TRAP!",
  "GOOD NIGHT SWEET SON OF THE GOD!!!",
  "SEE YA IN HELL!!!!",
  "BY, IRISHMEN!",
  "SWEET COLD NIGHTMARES TO YA!",
  "COLD",
  "111",
  "222",
  "333",
  "HERE COMES THE BROKEN DOOR! CLOSED IT IS FOR EVER MORE",
  "AHAHAHAHA!!!!!",
  "111111!!!!!",
  "0 !",
  "DEATH IS OTHER WAY!!!!",
  "RUSSIANS WILL SURRENDER NOT!",
  "FREQUENCY IS UNKNOWN",
  "RADIO FREQUENCY GLITCH",
  "TURMOIL",
  "DEAD FOREST",
  "CHOOSE THE SUB PROGRAM",
  "calling reinforcements",
  "calcification...",
  "decalcification...",
  "information extraction...",
  "YOU ARE NOT IN CANSAS ANY MORE!!!",
  "WELCOME TO THE CLUB FUCKER!",
  "THEY ARE COMING I GUESS!!!",
  "ALL CLEAR",
  "NO TARGET AVAILABLE",
  "YOUR ARE LOST",
  "IN THE WOODS",
  "DEATH AND LONLINESS",
  "ALSO FUCK U-SELF 4 DEAD",
  "GOLD INSIDE RAT OUTSIDE",
  "GO PLAY TESEY",
  "FIND YOUR GIRLFRIENDS STRING YOU LITTLE BUGGER",
  "666 666 666!!!",
  "WEELCOME TO THE LAND OF THE DEAD CHUMMER!",
  "BLACK GODDES IS DEAD",
  "DED BY THE HAND OF FATE!",
  "WEGHTED ON THE COURT OF RIGHT AND WRONG",
  "EXECUTED WITH THE FIRY SWORD!!!!!",
  "UHAHAHAHAHAHJA",
  "PSSSST!!! DEATH IS CO-OMI-I-ING!!!!!!"
};


void showPhrase(lcdPrint lcd) {
  char displayStr[16];
  Serial.println("Sizeof phrases");
  Serial.print(sizeof(phrases) / sizeof(char *));
  Serial.println("Random value");
  int getStr = random(sizeof(phrases) / sizeof(char *) - 1);
  Serial.print(getStr);
  int stringLength = strlen(phrases[getStr]);
  if (stringLength < 17) {
    for (int i = 0; i < stringLength; i++) {
      displayStr[i] = phrases[getStr][i];
    }
    if (stringLength < 16) {
      displayStr[stringLength] = NULL;
    }
    lcd.printLine(1, displayStr);
    lcd.printLine(2, "");
    return;
  }
  if (stringLength < 33) {
    for (int i = 0; i < 16; i++) {
      displayStr[i] = phrases[getStr][i];
    }
    lcd.printLine(1, displayStr);

    for (int i = 0; i < stringLength - 16; i++) {
      displayStr[i] = phrases[getStr][i + 16];
    }
    if (stringLength < 32) {
      displayStr[stringLength - 16] = NULL;
    }
    lcd.printLine(2, displayStr);
    return;
  }
  int diff = stringLength - 32;
  for (int offset = 0; offset <= diff; offset++) {

    for (int i = 0; i < 16; i++) {
      displayStr[i] = phrases[getStr][i + offset];
    }
    lcd.printLine(1, displayStr);

    for (int i = 0; i < 16; i++) {
      displayStr[i] = phrases[getStr][i + 16 + offset];
    }
    lcd.printLine(2, displayStr);
    delay(1000);
  }
}
