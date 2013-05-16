## Viikkoraportti 1

### Mitä opin
C99 standardista on jätetty pois monia yleishyödyllisiä funktioita ja vakioita, kuten esimerkiksi piin likiarvon sisältänyt ```M_PI``` ja luvun itseisarvon palauttava ```abs```-funktio eivät enää sisälly siihen. Opin myöskin että GNU Autotools käännösympäristö on auttamattoman monimutkainen, ja luultavasti liian työläs tämän kokoluokan projektia varten. En myöskään ennen ollut hoksannut, että JPEG-standardi määrittelee yksiselitteisen kvantisointimatriisin kuvadatan taajussisällön karsintaa varten. Tämä oli positiivinen yllätys, sillä nyt voin kokeilla erilaisia tarkuuksia eri taajuusalueille, ja mahdollisesti saavuttaa parempia tuloksia kuin standardia noudattava JPEG-pakkaus.

### Mikä jäi epäselväksi
Järkevä yksikkötestaus C:llä. Ajattelin että se olisi lähes samanlaista kuin Javalla tai Pythonilla, mutta C:llä ongelmaksi muodostuu pääsy käännösyksikön sisäisiin muuttujiin ja funktioihin. Vaihtoehdot ovat seuraavat:

1. Varsinaisen ohjelmakoodin liittäminen samaan tiedostoon testien kanssa ```#include```-käskyllä, jolloin molemmat kuuluvat samaan käännösyksikköön.
2. Ohjelman jakaminen niin pieniin moduuleihin, ettei staattisiin funktioihin tule muuta kuin triviaalia toiminnallisuutta, jota ei tarvitse testata erikseen.

Kumpikaan vaihtoehto ei vaikuta erityisen houkuttelevalta, mutta olen kuitenkin kallistumassa jälkimmäisen puoleen, sillä tällöin ohjelman käännösprosessi on yksinkertaisempi.

### Ohjelman edistyminen
Ohjelma pystyy nyt laskemaan 8x8 matriisin diskreetin kosinimuunnoksen, sekä sen käänteisprosessin IDCT:n. Myös kvantisointimatriisin skaalaus toimii JPEG-standardin määrittämällä tavalla.

### Mitä seuraavaksi
Toteutan kuvien latauksen ja DCT:n suorittamisen isommille taulukoille. Tutustun myös tarkemmin taajusisällön kvantisointiin, ja mahdollisesti toteutan jo joitain osia kerroinmatriisin pakkauskoodista. Alan myös suunnittelmaan Huffman-koodauksen vaatimia funktioita ja tietorakenteita, mutta en aloita vielä niiden implementointia.

