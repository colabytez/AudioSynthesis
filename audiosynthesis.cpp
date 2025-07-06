#include "SDL2/SDL.h"
#include "SDL2/SDL_audio.h"
#include <math.h>

const int AMPLITUDE = 28000;      // Äänenvoimakkuus
const int SAMPLE_RATE = 44100;    // Näytteenottotaajuus (Hz)
const double FREQUENCY = 441.0; // Taajuus Hz

// Äänipuskuriin kirjoittava callback-funktio
void sine_wave_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{   
    Sint16 *buffer = (Sint16*)raw_buffer;         // Muutetaan puskurin tyyppi 16-bittiseksi
    int length = bytes / 2;                       // Lasketaan näytteiden määrä (2 tavua per näyte)
    int &sample_nr = *(int*)user_data;            // Haetaan nykyinen näytenumero käyttäjädatan kautta

    for(int i = 0; i < length; i++, sample_nr++)  // Käydään puskuri läpi näyte kerrallaan
    {
        double time = (double)sample_nr / (double)SAMPLE_RATE; // Lasketaan aika sekunteina
        buffer[i] = (Sint16)(AMPLITUDE * sin(2.0 * M_PI * FREQUENCY * time)); // Käytetään muuttujaa
    }
}

void square_wave_callback(void *user_data, Uint8 *raw_buffer, int bytes)
{
    Sint16 *buffer = (Sint16*)raw_buffer;         // Muutetaan puskurin tyyppi 16-bittiseksi
    int length = bytes / 2;                       // Lasketaan näytteiden määrä (2 tavua per näyte)
    int &sample_nr = *(int*)user_data;            // Haetaan nykyinen näytenumero käyttäjädatan kautta

    for(int i = 0; i < length; i++, sample_nr++)  // Käydään puskuri läpi näyte kerrallaan
    {
        double time = (double)sample_nr / (double)SAMPLE_RATE; // Lasketaan aika sekunteina
        buffer[i] = (Sint16)(AMPLITUDE * (sin(2.0 * M_PI * FREQUENCY * time) >= 0 ? 1 : -1)); // Käytetään muuttujaa
    }
}

int main() {

    if(SDL_Init(SDL_INIT_AUDIO) < 0) {                                  // Alustetaan SDL
        SDL_Log("SDL:n alustus epäonnistui: %s", SDL_GetError());       // Virheviesti
        return -1;                                                      // Lopetetaan, jos epäonnistuu
    }

    int sample_nr = 0;                                                  // Näytenumero, jota käytetään callbackissa

    // Määritellään audiospesifikaatiot, joita käytetään äänen toistossa
    // Saadut audiospesifikaatiot määrittävät, miten ääni toistetaan
    // Halutut audiospesifikaatiot määrittävät, mitä ominaisuuksia halutaan äänen toistossa
    // Käytetään 44100 Hz näytteenottotaajuutta, 16-bittistä signed näytettä ja monoääntä
    // Puskuri on 2048 näytteen kokoinen, mikä on yleinen koko äänen toistossa
    // Callback-funktio on määritetty kirjoittamaan ääntä puskuriin
    // Käyttäjädatan osoite on asetettu näytenumeroon, jotta callback voi käyttää sitä
    // Käytetään SDL:n audio API

    SDL_AudioSpec obtained_specification;                               // Saadut audiospesifikaatiot // Ei tarvitse alustaa kenttiä
    
    SDL_AudioSpec desired_specification;                                // Halutut audiospesifikaatiot
    desired_specification.freq = SAMPLE_RATE;                           // Näytteenottotaajuus
    desired_specification.format = AUDIO_S16SYS;                        // 16-bittinen signed näyte
    desired_specification.channels = 1;                                 // Mono
    desired_specification.samples = 2048;                               // Puskurin koko
    desired_specification.callback = sine_wave_callback;                // Callback-funktio
    desired_specification.userdata = &sample_nr;                        // Käyttäjädatan osoite

    // Avataan audiolaite halutuilla spesifikaatioilla
    // Jos audiolaitteen avaaminen epäonnistuu, tulostetaan virhe
    // ja lopetetaan ohjelma
    if(SDL_OpenAudio(&desired_specification, &obtained_specification) < 0) { 
        SDL_Log("Audiolaitteen avaaminen epäonnistui: %s", SDL_GetError());
        return -1;  // Lopetetaan, jos epäonnistuu
    }

    SDL_PauseAudio(0);                                      // Käynnistetään ääni
    SDL_Delay(1000);                                        // Odotetaan 1 sekunti (ääni soi)
    SDL_PauseAudio(1);                                      // Pysäytetään ääni

    SDL_CloseAudio();

    // Vaihdetaan callback-funktio siniaallon tuottamiseen
    desired_specification.callback = square_wave_callback;  // Vaihdetaan callback-funktio
    sample_nr = 0;                                          // Nollataan näytenumero
    if(SDL_OpenAudio(&desired_specification, &obtained_specification) < 0) { 
        SDL_Log("Audiolaitteen uudelleen avaaminen epäonnistui: %s", SDL_GetError());
        return -1;  // Lopetetaan, jos epäonnistuu
    }

    // Toistetaan ääntä uudelleen
    // Käytetään samaa audiolaitetta, mutta nyt se tuottaa sinia
    SDL_PauseAudio(0);                                      // Käynnistetään ääni uudelleen 
    SDL_Delay(1000);                                        // Odotetaan 1 sekunti (ääni soi)
    SDL_PauseAudio(1);                                      // Pysäytetään ääni uudelleen

    SDL_CloseAudio();                                       // Suljetaan audiolaite

    return 0;
}