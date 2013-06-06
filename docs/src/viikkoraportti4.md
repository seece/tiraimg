## Viikkoraportti 4


### Mitä opin
JPEG-standardissa eri pakkaustekniikoiden suhteet ovat paljon läheisemmät kuin odotin. RLE:hen käytettävä nollientäyttö toteutetaan varsinaisessa JPEG-pakkaajassa Huffmanin koodauksen avulla lisäämällä symbolitauluun tätä varten erikoismerkkejä. Oma toteutukseni ei ole yhtä tehokas, mutta se erottelee eri pakkausvaiheet selkeäksi omiksi, riippumattomiksi komponenteikseen.

### Mikä jäi epäselväksi
Kahden kuvan erovaisuuksien laskeminen mielekkäästi. Päädyin tässä kuvien yksittäisten pikselien värikanavien vertailuun, sekä koko kuvan absoluuttisen virheen keskiarvon laskemiseen. Ajattelin aluksi käyttää signaali/kohina-suhdetta apuna kuvien vertailussa, mutta sen laskeminen olikin paljon monimutkaisempaa kuin odotin, joten se jäi implementoimatta.

### Ohjelman edistyminen
Ohjelma pakkaa nyt kuvat häviöllisesti pienempään tilaan. Tekstikäyttöliittymä toimii oikein. Pakkaus ei kuitenkaan ole niin tehokas kuin se voisi olla, sillä se perustuu tällä hetkellä yksinomaan pitkien nollatavusarjojen eliminoimiseen. Huffman-koodaus ei vielä toimi, mutta olen kuitenkin suunnitellut ohjelman pakkaus- ja purkuvaiheen niin, ettei toimivan pakkauksen lisääminen aiheuta muutoksia muuhun koodiin.

### Mitä seuraavaksi
Virittelen Huffman-koodauksen vihdoin kuntoon. Kehitys tapahtuu tällä hetkellä git versionhallinnan ```feature-huffman```-haarassa, sillä en halua lisätä toimimatonta koodia ```master```-haaraan.

