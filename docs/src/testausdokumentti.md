## Mitä testattiin

### Automaattisesti testatut 
* Ruutuja käsittelevät funktiot (```block.h```) testattiin käsin kirjoitetuilla, sekä tietyillä siemenluvuilla arvoituilla satunnaissyötteillä.
* Diskreetti kosinimuunnos testattiin tarkasti yhden 8x8-ruudun kokoisella syötteellä, ja ulkoisesta lähteestä hankitulla odotetulla ulostulolla. Muunnosta testattiin myös suorittamalla kuvalle ensin DCT ja sen jälkeen käänteismuunnos IDCT, jolloin lopputuloksen pitäisi olla pyöristysvirheitä lukuunottamatta identtinen lähdekuvan kanssa.
* Pinototeutusta testattiin lisäämällä pinoon arvoja, ja sen jälkeen tarkastelemalla pinon datataulukon sisältöä.
* Bittipuskuria (```BitBuffer```) testattiin lisäämällä puskuriin arvoja, ja sen jälkeen vertaamalla puskurin varaaman taulukon ensimmäisten tavujen sisältöä käsinlaskettujen verrokkitavujen sisältöön.
* Yksinkertaisia puu-toimintoja (```trie.h```) testattiin tutkimalla pienistä syötteistä luotujen puiden ominaisuuksia (esim. lehtien- ja solmujen lukumäärä).
* Huffman-koodausta testattiin tarkastelemalla syötedatan luomaa symbolijakaumaa, sekä yksinkertaisella syötetestillä jossa varmistettiin että pakkauksen läpi kulkeva data ei muutu.

### Testisyötteet
Automaattisten testien kuvina käytettiin tiedostoja ```testdata/small.ppm``` ja ```testdata/tiny.ppm```. Testissä käytetyt yksittäiset matriisit on määritelty tiedostossa ```test/test_data.c```.

### Käsin testatut
* Tekstikäyttöliittymän toimivuus. 
* Pakkausalgoritmin toimivuus erikokoisilla kuvilla.
* Monimutkaisempien puualgoritmien oikeellisuus varmistettiin vertaamalla niitä käsin laskettuihin tuloksiin.

## Pakkauksen tehokkuus ja laatu
Listaukseen on liitetty mukaan vertailun vuoksi myös oikean JPEG-pakkauksen tulokset. Kuvien pakkaamiseen ja purkamiseen kuluva aika ei riipu pakkauksen laatukertoimesta. Ajat on mitattu AMD Phenom II X4-prosessorilla.

![Tallinnan Linnamüür, testikuva](muuri.png "")

Kuva                      kasvot.ppm      muuri.ppm       iso_katu.ppm
----------------------    -------------   ------------    ------------
__Mitat (px)__            32x32           512x384         1536x2048 
__Pakkausaika__           65 ms           2 s             30.9 s
__Purkuaika__             51 ms           2.5 s           40 s
__Koko (pakkaamaton)__    3.1 KB          576.1 KB        9.0 MB
__TIMG-koko, q: 70__      852 B           72.6 KB         719.3 KB
__TIMG-koko, q: 50__      651 B           53.0 KB         578.5 KB
__TIMG-koko, q: 25__      434 B           37.3 KB         480.1 KB
----------------------    --------------  ------------    ------------

Kuva                      kasvot.ppm      muuri.ppm       iso_katu.ppm
----------------------    --------------  ------------    ------------
__JPEG-koko, q: 70__      612 B           29.6 KB         287.1 KB 
__JPEG-koko, q: 50__      518 B           20.5 KB         191.4 KB 
__JPEG-koko, q: 25__      445 B           12.1 KB         108.6 KB 
----------------------    --------------  ------------    ------------

![Eri pakkaustasot vasemmalta lukien: alkuperäinen, 70, 50 ja 25.](muuri_pakkaus.png "")


Taulukossa esiintyvä q-arvo on kuvanlaadun määräävä quality-arvo, joka skaalaa kvantisoinnissa käytettävää jakajamatriisia. Skaalaus tapahtuu tiraimgissä samalla tavalla kuin JPEG:ssä, joten arvot ovat vertailukelpoisia.

![Kahden suurimman kuvan kokovertailu](kaavio.png "")

![Pienin testikuva on kooltaan 32x32 pikseliä.](kasvot.png "")


### Johtopäätös
Pakkaus tuottaa visuaalisesti miellyttäviä tuloksia, ja pienentää tiedostokokoa merkittävästi. Tehokkuus ei ole kuitenkaan samalla tasolla JPEG:n kanssa, sillä tiraimg tallentaa väridatan täydellä tarkkuudella. Pakkauksen häviötön osa ei ole myöskään yhtä edistynyt.

## Testien toteutus
Ohjelman automaattiset testit on kirjoitettu käyttäen CuTest-kirjastoa, joka tarjoaa aputoimintoja arvojen vertailuun sekä testikokonaisuuksien järjestelyyn ja suorittamiseen. Testit on toteutettu linkittämällä testiohjelmaan mukaan kaikki pääohjelman toiminnallisuus, ja tarkastelemalla eri funktioiden palauttamia arvoja. 

## Testien toistaminen
Ohjelman yksikkötestit voidaan toistaa komentamalla `make all && make check` projektin juurihakemistossa. Testit saattavat tulostaa joitakin omia status-viestejään standardivirtaan, mutta testauksen päätteeksi viimeisenä pitäisi tulostua viesti ```OK (46 tests)```, joka kertoo testien onnistuneen.
