class AudioPlayer {
  
  private: 
    String* objectTracks;
    bool newMessage; 
    char messageType;

    typedef void (*AudioFinishedHandler)();
    AudioFinishedHandler audioFinishedHandler;

    void sendCommand(String command,bool complete = true) {
      Serial1.print(command);
      if(complete) Serial1.write(13);              // CR (carriage return)
    }
    
  public: 
    int current;
    
    AudioPlayer(String* _objectTracks, AudioFinishedHandler _audioFinishedHandler) {
      objectTracks = _objectTracks;
      audioFinishedHandler = _audioFinishedHandler;
      newMessage = true;
      Serial1.begin(9600);
      
      //sendCommand("VRA");
    }
    void playObject(int object) {
      String track = objectTracks[object];
      
      #ifdef DEBUG_AUDIO_PLAYER
        Serial.print("  play: ");
        Serial.println(track);
      #endif
      
      //Serial1.print("VPF ");
      //Serial1.print(track);
      //Serial1.write(13);
      //Serial1.print("VPF 1.MP3");
      //Serial1.write(13);
      
      sendCommand("VPF ",false);
      sendCommand(track);
    }
    void update() {
      while (Serial1.available() > 0) {
        byte b = Serial1.read();
        if(b == 13) {
          #ifdef DEBUG_AUDIO_PLAYER_SERIAL 
            Serial.println("");
          #endif
          newMessage = true;
        } else {
          if(newMessage) {
            char c = b;
            #ifdef DEBUG_AUDIO_PLAYER_SERIAL 
              Serial.print(c);
            #endif
            newMessage = false;
            messageType = c;
            if(messageType == 'S') { // Stopped
              audioFinishedHandler();
            }
          }
          #ifdef DEBUG_AUDIO_PLAYER_SERIAL 
          else if (messageType == 'T') {
            Serial.print(b); 
            char c = b;
            Serial.print(' ');
          } else {
            char c = b;
            Serial.print(c);
          }
          #endif
        }
      }
    }
    
};

