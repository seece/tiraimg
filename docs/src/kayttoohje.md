# Käyttöohje

## Ohjelman kääntäminen
Harjoitustyö käännettään käyttäen ```make```-ohjelmaa. Ohjelman voi kääntää komentamalla projektin juurihakemistossa seuraavasti:

	$ make all

Jotta kääntäminen onnistuisi, tulee järjestelmässä olla asennettuna paketti ```libnetpbm10-dev```, sekä C-käännösympäristö.

Nyt hakemistoon tulisi ilmestyä binääri nimeltä ```tiraimg```. Ohjelman voi nyt suorittaa:

	$ ./tiraimg -V
	tiraimg version 0.2

Tämän ohjeen myöhemissä osissa oletataan, että binääri on lisätty järjestelmän etsintäpolkuun, jolloin ohjelman voi suorittaa komentamalla pelkästään ```tiraimg```.

## Kuvien pakkaus
Tiraimg pakkaa kuvat omaan formaattiinsa. Syötekuvan on oltava PPM-formaatissa käyttöä varten, ja myös purkajan tuottama kuva on PPM-muotoinen. Kuva pakataan komentamalla ```tiraimg -q LAATU LÄHDE KOHDE```, missä ```LAATU``` on pakkauksen laatukerroin väliltä [1, 100], ```LÄHDE``` on PPM-muotoisen syötekuvan tiedostonimi ja ```KOHDE``` pakatun kohdekuvan tiedostonimi. Esim. kuvan tyty.ppm pakkaaminen voisi tapahtua seuraavasti:

	$ tiraimg -q 50 tyty.ppm tyty.timg

Huomaa, että tiedostoformaatin rajoitteiden takia korkeat laatukertoimet (yli 70) saattavat aiheuttaa joillakin syötteillä kuviin näkyviä artefakteja. 

### Kuvien muuttaminen PPM-muotoon

ImageMagick-paketin ```convert```-komennolla pystyy muuttamaan lähes minkä tahansa kuvan PPM:ksi. Esimerikki:

	$ convert lomakuva.jpg lomakuva.ppm

## Kuvien purkaminen
Pakatun kuvan saa purettua takaisin PPM-muotoon komentamalla ```tiraimg -d LÄHDE KOHDE```, missä ```-d``` valintalippu kytkee päälle kuvan pakkauksen purkamisen, ```LÄHDE``` on pakattu timg-kuva ja ```KOHDE``` on puretun PPM-kuvan haluttu tiedostonimi. Esimerkiksi samaisen tyty.timg-kuvan purkaminen tapahtuu seuraavasti:

	$ tiraimg -d tyty.timg tyty_output.ppm

## Lisäominaisuudet
Ohjelma tukee myös seuraavia lisäparametreja:

-h
:	Englanninkielinen pikaohje.
-v
:	Ohjelma tulostaa enemmän statusviestejä. 
-y
:	Älä muuta kuvaa takaisin RGB-muotoon purkamisvaiheessa. Hyödyllinen lähinnä testauskäytössä.
-i
:	Älä suorita kuvalle käänteismuunnosta. Kuvaan jäävät näkyviin DCT-kertoimet.
-t
:	Tulosta purkamiseen käytetty Huffman-puu.


## Ohjelman sisäinen ohje
Ohjelma tulostaa englanninkieliset käyttöohjeet komentamalla ```tiraimg -h```. 
