#include "SDL2/SDL.h"
#include "SDL2/SDL_audio.h"
#include <math.h>

const int AMPLITUDE = 28000;      // Äänenvoimakkuus
const int SAMPLE_RATE = 44100;    // Näytteenottotaajuus (Hz)
const double FREQUENCY = 441.0; // Taajuus Hz

// Syntetisoi siniaallon näytteet puskuriin
void sine_wave_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    auto buffer = reinterpret_cast<Sint16*>(raw_buffer); // 16-bittinen puskurimuoto
    int length = bytes / sizeof(Sint16);                 // Näytteiden määrä
    int &sample_nr = *reinterpret_cast<int*>(user_data); // Näytenumero viitteenä

    for(int i = 0; i < length; ++i, ++sample_nr) {
        double t = static_cast<double>(sample_nr) / SAMPLE_RATE;
        buffer[i] = static_cast<Sint16>(AMPLITUDE * sin(2.0 * M_PI * FREQUENCY * t));
    }
}

// Syntetisoi kanttiaallon näytteet puskuriin
void square_wave_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    auto buffer = reinterpret_cast<Sint16*>(raw_buffer);
    int length = bytes / sizeof(Sint16);
    int &sample_nr = *reinterpret_cast<int*>(user_data);

    for(int i = 0; i < length; ++i, ++sample_nr) {
        double t = static_cast<double>(sample_nr) / SAMPLE_RATE;
        buffer[i] = static_cast<Sint16>(AMPLITUDE * (sin(2.0 * M_PI * FREQUENCY * t) >= 0 ? 1 : -1));
    }
}

int main() {
    
    // Tämä on pääohjelma, joka alustaa SDL:n ja avaa audiolaitteen
    // Se määrittelee halutut audiospesifikaatiot ja avaa audiolaitteen
    // Sitten se toistaa ääntä siniaallon ja kanttiaallon muodossa

    // Alustetaan SDL ja tarkistetaan, että se onnistuu
    // Jos alustus epäonnistuu, tulostetaan virheviesti ja lopetetaan ohjelma
    // SDL_Init(SDL_INIT_AUDIO) alustaa vain äänitoiminnot
    // SDL_Log käytetään virheiden tulostamiseen
    
    if(SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL:n alustus epäonnistui: %s", SDL_GetError());
        return -1; // Lopetetaan, jos epäonnistuu
    }
    
    int sample_nr = 0; // Näytenumero, jota käytetään callbackissa
    
    // Määritellään audiospesifikaatiot, joita käytetään äänen toistossa
    // Saadut audiospesifikaatiot määrittävät, miten ääni to
    // Halutut audiospesifikaatiot määrittävät, mitä ominaisuuksia halutaan äänen toistossa
    // Käytetään 44100 Hz näytteenottotaajuutta, 16
    // -bittistä signed näytettä ja monoääntä
    // Puskuri on 2048 näytteen kokoinen, mikä on yleinen
    // koko äänen toistossa
    // Callback-funktio on määritetty kirjoittamaan ääntä puskuriin
    // Käyttäjädatan osoite on asetettu näytenumeroon, 
    // jotta callback voi käyttää sitä
    // Käytetään SDL:n audio API
    
    SDL_AudioSpec obtained_specification; // Saadut audiospesifikaatiot, ei tarvitse alustaa kenttiä
    
    SDL_AudioSpec desired_specification;  // Halutut audiospesifikaatiot
    desired_specification.freq = SAMPLE_RATE; // Näytteenottotaajuus
    desired_specification.format = AUDIO_S16SYS; // 16-bittinen signed nä
    desired_specification.channels = 1; // Mono
    desired_specification.samples = 2048; // Puskurin koko
    desired_specification.callback = sine_wave_callback; // Callback-funktio
    desired_specification.userdata = &sample_nr; // Käyttäjädatan osoite
    
    // Avataan audiolaite halutuilla spesifikaatioilla
    // Jos audiolaitteen avaaminen epäonnistuu, tulostetaan virhe
    // ja lopetetaan ohjelma
    if(SDL_OpenAudio(&desired_specification, &obtained_specification) < 0) {
        SDL_Log("Audiolaitteen avaaminen epäonnistui: %s", SDL_GetError());
        return -1; // Lopetetaan, jos epäonnistuu
    }
    
    SDL_PauseAudio(0); // Käynnistetään ääni
    SDL_Delay(1000); // Odotetaan 1 sekunti (ääni soi)
    SDL_PauseAudio(1); // Pysäytetään ääni
    SDL_CloseAudio(); // Suljetaan audiolaite
    
    // Vaihdetaan callback-funktio kanttiaallon tuottamiseen
    desired_specification.callback = square_wave_callback; // Vaihdetaan callback-funktio
    sample_nr = 0; // Nollataan näytenumero
    
    if(SDL_OpenAudio(&desired_specification, &obtained_specification) < 0) {
        SDL_Log("Audiolaitteen uudelleen avaaminen epäonnistui: %s", SDL_GetError());
        return -1; // Lopetetaan, jos epäonnistuu
    }
    
    // Toistetaan ääntä uudelleen
    // Käytetään samaa audiolaitetta, mutta nyt se tuottaa kanttia
    SDL_PauseAudio(0); // Käynnistetään ääni uudelleen
    SDL_Delay(1000); // Odotetaan 1 sekunti (ääni soi)
    SDL_PauseAudio(1); // Pysäytetään ääni uudelleen
    SDL_CloseAudio(); // Suljetaan audiolaite
    
    return 0; // Palautetaan 0, jos kaikki onnistuu
}