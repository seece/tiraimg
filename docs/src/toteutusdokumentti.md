## Ohjelman toteutus

### Yleisrakenne
Ohjelma on jaettu useampaan moduliin, joista jotkut ovat riippuvaisia toisistaan. Funktioiden nimeämiskäytäntönä on käytetty tyyliä ```moduuli_funktionimi```. Ohjelmassa on tekstipohjainen käyttöliittymä, joka käyttää compress-moduulin tarjoamia toimintoja kuvien lataukseen ja tallennukseen. Compress-moduuli käyttää lähes kaikkien muiden moduulien tarjoamia ominaisuuksia, sillä se sisältään pakkauksen korkean tason toimintalogiikan. Varsinaiset algoritmit (DCT, ZRL-pakkaus ja Huffman-koodaus) on toteutettu erillisissä moduuleissaan. 

#### Moduulit
* bitbuf.h
	+ bitin tarkkuudella toimiva dynaaminen taulukko	
* block.h
	+ 8x8 pikselin ruutujen käsittely
* compress.h
	+ korkean tason kuvanpakkausfunktiot
* dct.h
	+ diskreetin kosinimuunnokset toteuttavat funktiot
* eks_math.h
	+ C99-standardista puuttuvien matemaattisten funktioiden määrittelyt
* huffman.h
	+ huffmanin-koodaukseen liittyvät funktiot (eivät vielä toimi)
* jpeg.h
	+ jpeg-kohtaiset vakiot
* tiraimg.h
	+ ohjelmakohtaiset vakiot
* trie.h
	+ binääripuun toteutus


### Saavutetut aika- ja tilavaativuudet
### Pakkauksen tehokkuus ja laatu
### Puutteet
