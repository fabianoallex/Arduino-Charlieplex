/*************************************************************************************************************
*******************************BIT ARRAY *********************************************************************
**************************************************************************************************************
mais informações aqui: http://fabianoallex.blogspot.com.br/2015/09/arduino-array-de-bits.html
**************************************************************************************************************/
class BitArray{
  private:
    int _num_bits;   //quantidade de bits a serem gerenciados
    int _num_bytes;  //quantidade de bytes utilizados para armazenar os bits a serem gerenciados
    byte * _bytes;   //array de bytes onde estaram armazenados os bits
  public:
    BitArray(int num_bits){
      _num_bits  = num_bits;
      _num_bytes = _num_bits/8 + (_num_bits%8 ? 1 : 0) + 1;
      _bytes = (byte *)(malloc( _num_bytes * sizeof(byte) ) );
    }
     
    void write(int index, byte value) {
      byte b = _bytes[ index/8 + (index%8 ? 1 : 0) ];
      unsigned int bit = index%8;
      if (value) { b |= (1 << bit); } else { b &= ~(1 << bit);  }
      _bytes[ index/8 + (index%8 ? 1 : 0) ] = b;
    }
     
    void write(byte value) {
      for(int j=0; j<_num_bytes;j++) { _bytes[j] = value ? B11111111 : B00000000;  } 
    }
     
    int read(int index) {
      byte b = _bytes[ index/8 + (index%8 ? 1 : 0) ];
      unsigned int bit = index%8;
      return (b & (1 << bit)) != 0;
    }
 
   ~BitArray(){ free( _bytes ); }
};
/*************************************************************************************************************
*******************************FIM BIT ARRAY *****************************************************************
**************************************************************************************************************/

class Charlieplexing {
  private:
    byte _numPins;       //numero de pinos utilizados
    int  * _pins;        //ponteiro para array com os pinos utilizados
    byte _ledsOnPerGroup;//quantidade de leds de um mesmo grupo que pode ser ligada ao mesmo tempo
    BitArray * _states;     //armazenas o estados do leds
  public:
    Charlieplexing(int * pins, byte numPins, byte _ledsOnPerGroup=2);
    int numLeds() { return _numPins * _numPins - _numPins; }
    void update(int delay_=10, int repeat=1);
    void turnOn(int i);  //liga led i
    void turnOff(int i);  //desliga led i
    void clear();
    byte getState(int i);  //retorna status led i
};

Charlieplexing::Charlieplexing(int * pins, byte numPins, byte ledsOnPerGroup){
  _ledsOnPerGroup = ledsOnPerGroup;
  _numPins = numPins;
  _pins    = pins;
  _states  = new BitArray(numLeds());
}

void Charlieplexing::turnOn(int i){
  if (i < numLeds()){
    _states->write(i, HIGH);
  }
}

void Charlieplexing::turnOff(int i){
  if (i < numLeds()){
    _states->write(i, LOW);
  }
}

byte Charlieplexing::getState(int i){
  if (i < numLeds()){
    return _states->read(i);
  }
}

void Charlieplexing::clear(){
  _states->write(LOW);
}

void Charlieplexing::update(int delay_, int repeat){
  if (repeat <=0) { repeat = 1; }
  for (int q=0; q<repeat; q++) {
    for (int i=0; i<_numPins; i++) {
      digitalWrite(_pins[i], LOW);
      pinMode(_pins[i], OUTPUT);

      byte cont = 0; //conta se há algum led a ser ligado

      for (int j=0;j<_numPins;j++){
        if (i != j) {
          pinMode(_pins[j], INPUT);
          digitalWrite(_pins[j], LOW);
        }
      }

      for (int j=0; j<_numPins; j++) {
        if (i != j) {
          int posLed = 0; 

          if (i>j){
            posLed = i * i - i + 2 * j + 1;  //baseado no calculo do numero triangular
          } else {
            posLed = j * j - j + 2 * i;      //baseado no calculo do numero triangular
          }

          if (_states->read(posLed) == HIGH){
            pinMode(_pins[j], OUTPUT);
            digitalWrite(_pins[j], LOW);
            cont++;
          } 
        }

        if (cont == _ledsOnPerGroup && cont > 0 ) {
          cont = 0;
          digitalWrite(_pins[i], HIGH);
          delay(delay_); 
          digitalWrite(_pins[i], LOW);

          for (int jj=0;jj<=j;jj++){
            if (i != jj) {
              pinMode(_pins[jj], INPUT);
            }
          }
        }
      }
      if (cont > 0){
        cont = 0;
        digitalWrite(_pins[i], HIGH);
        delay(delay_); 
        digitalWrite(_pins[i], LOW);
      }
    }
  }
}

int pins[] = {12, 11, 10, 9, 8};
int qtPins = 5;
int qtLedsOn = 2;

Charlieplexing cp(pins, qtPins, qtLedsOn);  //pinos, qtpinos, qtledsOn

void setup(){
  cp.turnOn(0);
  cp.turnOn(2);
  
  cp.turnOn(4);
  cp.turnOn(8);
  cp.turnOn(14);
  
  cp.turnOn(7);
  cp.turnOn(18);
  
  cp.turnOn(13);
  cp.turnOn(15);
  cp.turnOn(17);
  cp.turnOn(19);
}

void loop() { 
  cp.update(5, 20); //delay, repeat
}