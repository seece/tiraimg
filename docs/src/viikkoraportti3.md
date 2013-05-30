## Viikkoraportti 3

### Mitä opin
Luin gammakorjauksesta, ja opin että suurin osa web-selaimilla katsotuista kuvista on gammakorjattuja, eli niiden kirkkausarvot eivät ole lineaarisia, vaan niitä on skaalattu tietyllä eksponentilla lisätarkkuuden saavuttamiseksi. Tällä on runsaasti merkitystä kuvanpakkauksen kanssa, sillä jos korjaus suoritetaan esim. kaksi kertaa peräkkäin niin kuva vääristyy ja informaatiota menetetään. JPEG-standardissa kaikki kuvat ovat gammakorjattuja eksponentilla 2.2, ja tämä pätee myös omaan kuvaformaattiini.

### Mikä jäi epäselväksi
Huffman-koodauksen erityiskoodit. JPEG-pakkauksessa ruutujen taajuuskertoimien RLE-pakkaus on toteutettu erityismerkillä, joka tarkoittaa että ruudun loppuosa on pelkkää nollariviä, mutten löytänyt tästä kovin tarkkaa selitystä. Oma RLE-toteutukseni tallentaa ainoastaan ruudun datan pituuden tavuina, ja ruudun loppuosan arvojen oletetaan olevan nollia. Tämä ei ole yhtä tehokasta, mutta toteutus ei ole tällöin riippuvainen entropiakoodaajasta (eli tässä tapauksessa Huffman-koodauksesta).

### Ohjelman edistyminen
Tällä viikolla toteutin seuraavat ominaisuudet:

* komentorivikäyttöliittymä
* kokonaisen kuvan DCT (diskreetti kosinimuunnos taajuusesitysmuotoon)
* ruutujen informaation uudelleenjärjestely pakkautuvampaan esityismuotoon (```byteblock_pack```-funktio)
* alustava tiedostoformaatti, huffman-koodauksen tarvitsemat tiedot puuttuvat vielä
* pakatun kuvan tallennus levylle

### Mitä seuraavaksi
Seuraavaksi toteutan kuvan lataamisen levyltä, ja lopullisen datan Huffman-koodauksen. Ajattelin myös lisätä muutamia tilastollisia testejä (signaali-kohinasuhde yms.) pakkauksen laadun selvittämiseksi.

