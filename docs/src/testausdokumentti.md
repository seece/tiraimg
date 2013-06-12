## Mitä testattiin

### Automaattisesti testatut 
* Ruutuja käsittelevät funktiot (```block.h```) testattiin käsin kirjoitetuilla, sekä tietyillä siemenluvuilla arvoituilla satunnaissyötteillä.
* Diskreetti kosinimuunnos testattiin tarkasti yhden 8x8-ruudun kokoisella syötteellä, ja ulkoisesta lähteestä hankitulla odotetulla ulostulolla. Muunnosta testattiin myös suorittamalla kuvalle ensin DCT ja sen jälkeen käänteismuunnos IDCT, jolloin lopputuloksen pitäisi olla pyöristysvirheitä lukuunottamatta identtinen lähdekuvan kanssa.
* Pinototeutusta testattiin lisäämällä pinoon arvoja, ja sen jälkeen tarkastelemalla pinon datataulukon sisältöä.
* Bittipuskuria (```BitBuffer```) testattiin lisäämällä puskuriin arvoja, ja sen jälkeen vertaamalla puskurin varaaman taulukon ensimmäisten tavujen sisältöä käsinlaskettujen verrokkitavujen sisältöön.
* Yksinkertaisia puu-toimintoja (```trie.h```) testattiin tutkimalla pienistä syötteistä luotujen puiden ominaisuuksia (esim. lehtien- ja solmujen määrä).
* Huffman-koodausta testattiin tarkastelemalla syötedatan luomaa symbolijakaumaa, sekä yksinkertaisella syötetestillä jossa varmistettiin että pakkauksen läpi kulkeva data ei muutu.

### Testisyötteet
Automaattisten testien kuvina käytettiin tiedostoja ```testdata/small.ppm``` ja ```testdata/tiny.ppm```. Testissä käytetyt yksittäiset matriisit on määritelty tiedostossa ```test/test_data.c```.

### Käsin testatut
* Tekstikäyttöliittymän toimivuus. 
* Pakkausalgoritmin toimivuus erikokoisilla kuvilla.
* Monimutkaisempien puualgoritmien oikeellisuus varmistettiin vertaamalla niitä käsin laskettuihin tuloksiin.

## Testien toteutus
Ohjelman automaattiset testit on kirjoitettu käyttäen CuTest-kirjastoa, joka tarjoaa aputoimintoja arvojen vertailuun sekä testikokonaisuuksien järjestelyyn ja suorittamiseen. Testit on toteutettu linkittämällä testiohjelmaan mukaan kaikki pääohjelman toiminnallisuus, ja tarkastelemalla eri funktioiden palauttamia arvoja. 

## Testien toistaminen
Ohjelman yksikkötestit voidaan toistaa komentamalla `make all && make check` projektin juurihakemistossa. Testit saattavat tulostaa joitakin omia status-viestejään standardivirtaan, mutta testauksen päätteeksi viimeisenä pitäisi tulostua viesti ```OK (43 tests)```, joka kertoo testien onnistuneen.
