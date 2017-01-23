
#include <string.h>

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

//pocitame s FAT32 MAX - tedy horni 4 hodnoty
const int32_t FAT_UNUSED = INT32_MAX - 1;
const int32_t FAT_FILE_END = INT32_MAX - 2;
const int32_t FAT_BAD_CLUSTER = INT32_MAX - 3;
const int32_t FAT_DIRECTORY = INT32_MAX - 4;

#define FAT_SIZE  251
#define CLUSTER_SIZE 256

static pthread_mutex_t mutexes[FAT_SIZE];
/*
struct boot_record {
    char volume_descriptor[250];    //popis vygenerovaného FS
    int8_t fat_type;                //typ FAT (FAT12, FAT16...) 2 na fat_type - 1 clusterů
    int8_t fat_copies;              //počet kopií FAT tabulek
    int16_t cluster_size;           //velikost clusteru
    int32_t usable_cluster_count;   //max počet clusterů, který lze použít pro data (-konstanty)
    char signature[9];              //login autora FS
};// 272B
*/
//pokud bude ve FAT FAT_DIRECTORY, budou na disku v daném clusteru uloženy struktury o velikosti sizeof(directory) = 24B
struct directory{
    char name[13];                  //jméno souboru, nebo adresáře ve tvaru 8.3'/0' 12 + 1
    int8_t isFile;                    //identifikace zda je soubor (TRUE), nebo adresář (FALSE)
    int32_t size;                   //velikost položky, u adresáře 0
    int32_t start_cluster;          //počáteční cluster položky
};// 24B

/*
int buildTestFat(int argc, char** argv) {

    FILE *fp;
    struct boot_record br;
    struct directory root_a, root_b, root_c, root_d;

    //boot_rec
    //smazani vsech B na /0 u stringu
    memset(br.signature, '\0', sizeof(br.signature));
    memset(br.volume_descriptor, '\0', sizeof(br.volume_descriptor));
    strcpy(br.volume_descriptor, "Testiovaci - FAT8 - tedy 256 clusteru, cluster 256B - 4 rezervovane - pouzitelne 0-251 (zde neni nutne rezervovat)");
    br.fat_type = 8;
    br.fat_copies = 2;
    br.cluster_size = 256;
    br.usable_cluster_count = 251;
    strcpy(br.signature, "zmk");

    //directory - vytvoreni polozek
    memset(root_a.name, '\0', sizeof(root_a.name));
    root_a.isFile = 1;
    strcpy(root_a.name,"cisla.txt");
    root_a.size = 135;
    root_a.start_cluster = 1;

    memset(root_b.name, '\0', sizeof(root_b.name));
    root_b.isFile = 1;
    strcpy(root_b.name,"pohadka.txt");
    root_b.size = 5975;
    root_b.start_cluster = 2;

    memset(root_c.name, '\0', sizeof(root_c.name));
    root_c.isFile = 1;
    strcpy(root_c.name,"msg.txt");
    root_c.size = 396;
    root_c.start_cluster = 30;

    memset(root_d.name, '\0', sizeof(root_d.name));
    root_d.isFile = 0;
    strcpy(root_d.name,"direct-1");
    root_d.size = 0;
    root_d.start_cluster = 29;

    // pro zapis budu potrebovat i prazdny cluster
    char cluster_empty[br.cluster_size];
    ////////////////////////////////////////////// SOUBORY A ADRESARE POKUSNE
    char cluster_dir1[br.cluster_size];
    char cluster_a[br.cluster_size];
    char cluster_b1[br.cluster_size];
    char cluster_b2[br.cluster_size];
    char cluster_b3[br.cluster_size];
    char cluster_b4[br.cluster_size];
    char cluster_b5[br.cluster_size];
    char cluster_b6[br.cluster_size];
    char cluster_b7[br.cluster_size];
    char cluster_b8[br.cluster_size];
    char cluster_b9[br.cluster_size];
    char cluster_b10[br.cluster_size];
    char cluster_b11[br.cluster_size];
    char cluster_b12[br.cluster_size];
    char cluster_b13[br.cluster_size];
    char cluster_b14[br.cluster_size];
    char cluster_b15[br.cluster_size];
    char cluster_b16[br.cluster_size];
    char cluster_b17[br.cluster_size];
    char cluster_b18[br.cluster_size];
    char cluster_b19[br.cluster_size];
    char cluster_b20[br.cluster_size];
    char cluster_b21[br.cluster_size];
    char cluster_b22[br.cluster_size];
    char cluster_b23[br.cluster_size];
    char cluster_b24[br.cluster_size];
    char cluster_c1[br.cluster_size];
    char cluster_c2[br.cluster_size];

    //pripravim si obsah - delka stringu musi byt stejna jako velikost clusteru
    memset(cluster_empty, '\0', sizeof(cluster_empty));
    memset(cluster_a, '\0', sizeof(cluster_a));
    memset(cluster_b1, '\0', sizeof(cluster_b1));
    memset(cluster_b2, '\0', sizeof(cluster_b2));
    memset(cluster_b3, '\0', sizeof(cluster_b3));
    memset(cluster_b4, '\0', sizeof(cluster_b4));
    memset(cluster_b5, '\0', sizeof(cluster_b5));
    memset(cluster_b6, '\0', sizeof(cluster_b6));
    memset(cluster_b7, '\0', sizeof(cluster_b7));
    memset(cluster_b8, '\0', sizeof(cluster_b8));
    memset(cluster_b9, '\0', sizeof(cluster_b9));
    memset(cluster_b10, '\0', sizeof(cluster_b10));
    memset(cluster_b11, '\0', sizeof(cluster_b11));
    memset(cluster_b12, '\0', sizeof(cluster_b12));
    memset(cluster_b13, '\0', sizeof(cluster_b13));
    memset(cluster_b14, '\0', sizeof(cluster_b14));
    memset(cluster_b15, '\0', sizeof(cluster_b15));
    memset(cluster_b16, '\0', sizeof(cluster_b16));
    memset(cluster_b17, '\0', sizeof(cluster_b17));
    memset(cluster_b18, '\0', sizeof(cluster_b18));
    memset(cluster_b19, '\0', sizeof(cluster_b19));
    memset(cluster_b20, '\0', sizeof(cluster_b20));
    memset(cluster_b21, '\0', sizeof(cluster_b21));
    memset(cluster_b22, '\0', sizeof(cluster_b22));
    memset(cluster_b23, '\0', sizeof(cluster_b23));
    memset(cluster_b24, '\0', sizeof(cluster_b24));
    memset(cluster_c1, '\0', sizeof(cluster_c1));
    memset(cluster_c2, '\0', sizeof(cluster_c2));
    memset(cluster_dir1, '\0', sizeof(cluster_dir1));
    strcpy(cluster_a,"0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789 - tohle je malicky soubor pro test");
    strcpy(cluster_b1, "Byla jednou jedna sladka divenka, kterou musel milovat kazdy, jen ji uvidel, ale nejvice ji milovala jeji babicka, ktera by ji snesla i modre z nebe. Jednou ji darovala cepecek karkulku z cerveneho sametu a ten se vnucce tak libil, ze nic jineho nechtela ");
    strcpy(cluster_b2, "nosit, a tak ji zacali rikat Cervena Karkulka. Jednou matka Cervene Karkulce rekla: „Podivej, Karkulko, tady mas kousek kolace a lahev vina, zanes to babicce, je nemocna a zeslabla, timhle se posilni. Vydej se na cestu drive nez bude horko, jdi hezky spor");
    strcpy(cluster_b3, "adane a neodbihej z cesty, kdyz upadnes, lahev rozbijes a babicka nebude mit nic. A jak vejdes do svetnice, nezapomeň babicce poprat dobreho dne a ne abys smejdila po vsech koutech.“ „Ano, maminko, udelam, jak si prejete.“ rekla Cerveni Karkulka, na stvrz");
    strcpy(cluster_b4, "eni toho slibu podala matce ruku a vydala se na cestu. Babicka bydlela v lese; celou půlhodinu cesty od vesnice. Kdyz sla Cervena Karkulka lesem, potkala vlka. Tenkrat jeste nevedela, co je to za zaludne zvire a ani trochu se ho nebala. „Dobry den, Cerven");
    strcpy(cluster_b5, "a Karkulko!“ rekl vlk. „Dekuji za prani, vlku.“ „Kampak tak casne, Cervena Karkulko?“ „K babicce!“ „A copak to neses v zasterce?“ „Kolac a vino; vcera jsme pekli, nemocne a zeslable babicce na posilnenou.“ „Kdepak bydli babicka, Cervena Karkulko?“ „Inu, j");
    strcpy(cluster_b6, "este tak ctvrthodiny cesty v lese, jeji chaloupka stoji mezi tremi velkymi duby, kolem je liskove oresi, urcite to tam musis znat.“ odvetila Cervena Karkulka. Vlk si pomyslil: „Tohle mlaďoucke, jemňoucke masicko bude jiste chutnat lepe nez ta starena, mus");
    strcpy(cluster_b7, "im to navleci lstive, abych schlamstnul obe.“ Chvili sel vedle Cervene Karkulky a pak pravil: „Cervena Karkulko, koukej na ty krasne kvetiny, ktere tu rostou vsude kolem, procpak se trochu nerozhlednes? Myslim, ze jsi jeste neslysela ptacky, kteri by zpiv");
    strcpy(cluster_b8, "ali tak libezne. Ty jsi tu vykracujes, jako kdybys sla do skoly a pritom je tu v lese tak krasne!“ Cervena Karkulka otevrela oci dokoran a kdyz videla, jak slunecni paprsky tancuji skrze stromy sem a tam a vsude roste tolik krasnych kvetin, pomyslila si: ");
    strcpy(cluster_b9, "„Kdyz prinesu babicce kytici cerstvych kvetin, bude mit jiste radost, casu mam dost, prijdu akorat.“ A sebehla z cesty do lesa a trhala kvetiny. A kdyz jednu utrhla, zjistila, ze o kus dal roste jeste krasnejsi, bezela k ni, a tak se dostavala stale hloub");
    strcpy(cluster_b10,"eji do lesa. Ale vlk bezel rovnou k babiccine chaloupce a zaklepal na dvere. „Kdo je tam?“ „Cervena Karkulka, co nese kolac a vino, otevri!“ „Jen zmackni kliku,“ zavolala babicka: „jsem prilis slaba a nemohu vstat.“ Vlk vzal za kliku, otevrel dvere a beze");
    strcpy(cluster_b11,"slova sel rovnou k babicce a spolknul ji. Pak si obleknul jeji saty a nasadil jeji cepec, polozil se do postele a zatahnul zaves. Zatim Cervena Karkulka behala mezi kvetinami, a kdyz jich mela naruc tak plnou, ze jich vic nemohla pobrat, tu ji prisla na  ");
    strcpy(cluster_b12,"mysl babicka, a tak se vydala na cestu za ni. Podivila se, ze jsou dvere otevrene, a kdyz vesla do svetnice, prislo ji vse takove podivne, ze si pomyslila: „Dobrotivy Boze, je mi dneska nejak úzko a jindy jsem u babicky tak rada.“ Zvolala: „Dobre jitro!“ ");
    strcpy(cluster_b13,"Ale nedostala zadnou odpoveď. Šla tedy k posteli a odtahla zaves; lezela tam babicka a mela cepec narazeny hluboko do obliceje a vypadala nejak podivne. Ach, babicko, proc mas tak velke usi?“ „Abych te lepe slysela.“ „Ach, babicko, proc mas tak velke oci ");
    strcpy(cluster_b14,"?“ „Abych te lepe videla.“ „Ach, babicko, proc mas tak velke ruce?“ „Abych te lepe objala.“ „Ach, babicko, proc mas tak straslivou tlamu?“ „Abych te lepe sezrala!!“ Sotva vlk ta slova vyrknul, vyskocil z postele a ubohou Cervenou Karkulku spolknul. Kdyz t");
    strcpy(cluster_b15,"eď uhasil svoji zadostivost, polozil se zpatky do postele a usnul a z toho spanku se jal mocne chrapat. Zrovna sel kolem chaloupky lovec a pomyslil si: „Ta starenka ale chrape, musim se na ni podivat, zda neco nepotrebuje.“ Vesel do svetnice, a kdyz prist");
    strcpy(cluster_b16,"oupil k posteli, uvidel, ze v ni lezi vlk. „Tak prece jsem te nasel, ty stary hrisniku!“ zvolal lovec: „Uz mam na tebe dlouho policeno!“ Strhnul z ramene pusku, ale pak mu prislo na mysl, ze vlk mohl sezrat babicku a mohl by ji jeste zachranit. Nestrelil ");
    strcpy(cluster_b17,"tedy, nybrz vzal nůzky a zacal spicimu vlkovi parat bricho. Sotva ucinil par rezů, uvidel se cervenat karkulku a po par dalsich rezech vyskocila divenka ven a volala: „Ach, ja jsem se tolik bala, ve vlkovi je cernocerna tma.“ A potom vylez la ven i ziva b");
    strcpy(cluster_b18,"abicka; sotva dechu popadala. Cervena Karkulka pak nanosila obrovske kameny, kterymi vlkovo bricho naplnili, a kdyz se ten probudil a chtel uteci, kameny ho tak desive tizily, ze klesnul k zemi nadobro mrtvy. Ti tri byli spokojeni. Lovec stahnul vlkovi ko");
    strcpy(cluster_b19,"zesinu a odnesl si ji domů, babicka snedla kolac a vypila vino, ktere Cervena Karkulka prinesla, a opet se zotavila. A Cervena Karkulka? Ta si svatosvate prisahala: „Uz nikdy v zivote nesejdu z cesty do lesa, kdyz mi to maminka zakaze!“ O Cervene  Karkulc");
    strcpy(cluster_b20,"e se jeste vypravi, ze kdyz sla jednou zase k babicce s babovkou, potkala jineho vlka a ten se ji taky vemlouval a snazil se ji svest z cesty. Ale  ona se toho vystrihala a kracela rovnou k babicce, kde hned vypovedela, ze potkala vlka, ktery ji sice popr");
    strcpy(cluster_b21,"al dobry den, ale z oci mu koukala nekalota. „Kdyby to nebylo na verejne ceste, jiste by mne sezral!“ „Pojď,“  rekla babicka: „zavreme dobre dvere, aby nemohl dovnitr.“ Brzy nato zaklepal vlk a zavolal: „Otevri, babicko, ja jsem Cervena Karkulka a nesu ti");
    strcpy(cluster_b22,"pecivo!“ Ty dve vsak zůstaly jako peny a neotevrely. Tak se ten sedivak plizil kolem domu a naslouchal, pak vylezl na strechu, aby tam pockal, az Cervena Karkulka půjde vecer domů, pak ji v temnote popadne a sezere. Ale babicka zle vlkovy úmysly odhalila ");
    strcpy(cluster_b23,". Pred domem staly obrovske kamenne necky, tak Cervene  Karkulce rekla: „Vezmi vedro, devenko, vcera jsem varila klobasy, tak tu vodu nanosime venku do necek.“ Kdyz byly necky plne, stoupala vůne klobas nahoru az k vlkovu cenichu. Zavetril a natahoval krk");
    strcpy(cluster_b24,"tak daleko, ze se na strese vice neudrzel a zacal klouzat dolů, kde spadnul primo do necek a bidne se utopil.");
    strcpy(cluster_c1, "Prodej aktiv SABMilleru v Ceske republice, Polsku, Maďarsku, Rumunsku a na Slovensku je soucasti podminek pro prevzeti podniku ze strany americkeho pivovaru Anheuser-Busch InBev, ktere bylo dokonceno v rijnu. Krome Plzeňskeho Prazdroje zahrnuji prodavanas");
    strcpy(cluster_c2, "aktiva polske znacky Tyskie a Lech, slovensky Topvar, maďarsky Dreher a rumunsky Ursus. - Tento soubor je sice kratky, ale neni fragmentovany");

/////////// ZACATEK VYTVARENI FAT TABULKY

    int32_t fat[br.usable_cluster_count];

    //ve fatce na 0 clusteru bude root directory
    fat[0] = FAT_DIRECTORY;
    //pak bude soubor "cisla.txt" - ten je jen jednoclusterovy
    fat[1] = FAT_FILE_END;
    //pak bude dlouhy soubor "pohadka.txt", ktery je cely za sebou
    fat[2] = 3;
    fat[3] = 4;
    fat[4] = 5;
    fat[5] = 6;
    fat[6] = 7;
    fat[7] = 8;
    fat[8] = 9;
    fat[9] = 10;
    fat[10] = 11;
    fat[11] = 12;
    fat[12] = 13;
    fat[13] = 14;
    fat[14] = 15;
    fat[15] = 16;
    fat[16] = 17;
    fat[17] = 18;
    fat[18] = 19;
    fat[19] = 20;
    fat[20] = 21;
    fat[21] = 22;
    fat[22] = 23;
    fat[23] = 24;
    fat[24] = 25;
    fat[25] = FAT_FILE_END;
    //ted bude nejake volne misto
    fat[26] = FAT_UNUSED;
    fat[27] = FAT_UNUSED;
    fat[28] = FAT_UNUSED;
    //pak adresar "directory-1"
    fat[29] = FAT_DIRECTORY;
    //pak soubor "msg.txt"
    fat[30] = 33;
    //bohuzel pri jeho zapisu se se muselo fragmenotvat - jsou tu dva spatne sektory
    fat[31] = FAT_BAD_CLUSTER;
    fat[32] = FAT_BAD_CLUSTER;
    //a tady je konec "msg.txt"
    fat[33] = FAT_FILE_END;
    //zbytek bude prazdny
    for (int32_t i = 34; i < br.usable_cluster_count; i++)
    {
        fat[i] = FAT_UNUSED;
    }

/////////// KONEC VYTVARENI FAT TABULKY


    fp = fopen("empty.fat", "w");
    //boot record
//        fwrite(&br, sizeof(br), 1, fp);
    // 2x FAT
    fwrite(&fat, sizeof(fat), 1, fp);
    fwrite(&fat, sizeof(fat), 1, fp);
    //dir - bacha, tady je potreba zapsat CELY CLUSTER, ne jen prvnich n-BYTU plozek - tedy doplnit nulami poradi zaznamu v directory NEMUSI odpovidat poradi ve FATce a na disku
    int16_t cl_size = br.cluster_size;
    int16_t ac_size = 0;
    fwrite(&root_a, sizeof(root_a), 1, fp);
    ac_size += sizeof(root_a);
    fwrite(&root_b, sizeof(root_b), 1, fp);
    ac_size += sizeof(root_b);
    fwrite(&root_c, sizeof(root_c), 1, fp);
    ac_size += sizeof(root_c);
    fwrite(&root_d, sizeof(root_d), 1, fp);
    ac_size += sizeof(root_d);
    char buffer[] = { '\0' };
    for (int16_t i = 0; i < (cl_size - ac_size); i++)
        fwrite(buffer, sizeof(buffer), 1, fp);
    //data - soubor cisla.txt
    fwrite(&cluster_a, sizeof(cluster_a), 1, fp);
    //data - soubor pohadka.txt
    fwrite(&cluster_b1, sizeof(cluster_b1), 1, fp);
    fwrite(&cluster_b2, sizeof(cluster_b2), 1, fp);
    fwrite(&cluster_b3, sizeof(cluster_b3), 1, fp);
    fwrite(&cluster_b4, sizeof(cluster_b4), 1, fp);
    fwrite(&cluster_b5, sizeof(cluster_b5), 1, fp);
    fwrite(&cluster_b6, sizeof(cluster_b6), 1, fp);
    fwrite(&cluster_b7, sizeof(cluster_b7), 1, fp);
    fwrite(&cluster_b8, sizeof(cluster_b8), 1, fp);
    fwrite(&cluster_b9, sizeof(cluster_b9), 1, fp);
    fwrite(&cluster_b10, sizeof(cluster_b10), 1, fp);
    fwrite(&cluster_b11, sizeof(cluster_b11), 1, fp);
    fwrite(&cluster_b12, sizeof(cluster_b12), 1, fp);
    fwrite(&cluster_b13, sizeof(cluster_b13), 1, fp);
    fwrite(&cluster_b14, sizeof(cluster_b14), 1, fp);
    fwrite(&cluster_b15, sizeof(cluster_b15), 1, fp);
    fwrite(&cluster_b16, sizeof(cluster_b16), 1, fp);
    fwrite(&cluster_b17, sizeof(cluster_b17), 1, fp);
    fwrite(&cluster_b18, sizeof(cluster_b18), 1, fp);
    fwrite(&cluster_b19, sizeof(cluster_b19), 1, fp);
    fwrite(&cluster_b20, sizeof(cluster_b20), 1, fp);
    fwrite(&cluster_b21, sizeof(cluster_b21), 1, fp);
    fwrite(&cluster_b22, sizeof(cluster_b22), 1, fp);
    fwrite(&cluster_b23, sizeof(cluster_b23), 1, fp);
    fwrite(&cluster_b24, sizeof(cluster_b24), 1, fp);
    //3x volne misto
    fwrite(&cluster_empty, sizeof(cluster_empty), 1, fp);
    fwrite(&cluster_empty, sizeof(cluster_empty), 1, fp);
    fwrite(&cluster_empty, sizeof(cluster_empty), 1, fp);
    //prazdny adresar
    fwrite(&cluster_empty, sizeof(cluster_empty), 1, fp);
    //prvni cast msg.txt
    fwrite(&cluster_c1, sizeof(cluster_c1), 1, fp);
    //sem je jedno co zapisu, sou to vadne sektory - tedy realne byto meli byt stringy FFFFFFcosi cosi cosiFFFFFF
    fwrite(&cluster_empty, sizeof(cluster_empty), 1, fp);
    fwrite(&cluster_empty, sizeof(cluster_empty), 1, fp);
    //druha cast msg.txt
    fwrite(&cluster_c2, sizeof(cluster_c2), 1, fp);
    //zbytek disku budou 0
    for (long i=34; i < br.usable_cluster_count; i++)
    {
        fwrite(&cluster_empty, sizeof(cluster_empty), 1, fp);
    }
    fclose(fp);



    return 0;
}
*/

/**
 * Procedura která překládá číslo FAT na speciální hodnoty
 *
 * */
char* printFat(int32_t fatValue){
    static char result[10];
    memset(result, '\0', sizeof(result));
    if(fatValue == FAT_UNUSED){
        return "EMP";
    }else if(fatValue == FAT_FILE_END){
        return "END";
    }else if(fatValue == FAT_BAD_CLUSTER) {
        return "BAD";
    }else if(fatValue == FAT_DIRECTORY){
        return "DIR";
    }else{
        snprintf(result, sizeof(result), "%d", fatValue);
        return result;
    }

};

/**
 * Vypise vsechny podslazky slozky, rekurzivne
 *
 * */
void printDirectoryPath(char clusters[FAT_SIZE], struct directory* dir, int* tabs){
    int i;
    while(strlen(dir->name) > 0){
        for (i = 0; i < *tabs; ++i) {
            printf("\t");
        }
        if(dir->isFile){
            printf("-%s %d %d\n", dir->name, dir->start_cluster, (int)ceil(dir->size/255.0));
        }else{
            printf("+%s\n", dir->name);
            *tabs = *tabs + 1;
            printDirectoryPath(clusters, (struct directory*)(clusters + dir->start_cluster * CLUSTER_SIZE), tabs);
        }
        dir++;
    }
    *tabs = *tabs - 1;
    for (i = 0; i < *tabs; ++i) {
        printf("\t");
    }
    printf("--\n");
}

/**
 * Vypise strom souboroveho systemu
 *
 * */
void printRoot(char clusters[FAT_SIZE]){

    int tabs = 0;
    struct directory* dir = (struct directory*)clusters;
    if(strlen(dir->name) == 0){
        printf("EMPTY\n");
        return;
    }else{
        printf("+ROOT\n");
        tabs++;
    }
    printDirectoryPath(clusters, dir, &tabs);

}

/**
 * Vypise vsechny nenulove clustery s jejich obsahem
 *
 * */
void printAll(int32_t fat1[FAT_SIZE], char clusters[FAT_SIZE]){
    int i, j;
    for (i = 0; i < FAT_SIZE; i++) {
        if(fat1[i] != FAT_UNUSED) {
            printf("%d:\t%s\n", i, printFat(fat1[i]));
            for (j = 0; j < CLUSTER_SIZE; ++j) {
                printf("%c", (clusters + i * CLUSTER_SIZE + j)[0]);
            }
            printf("\n");
        }
    }
}

/**
 * Vrati prvni volny cluster
 *
 * */
int32_t getEmptyCluster(int32_t fat1[FAT_SIZE]){
    int i = 0;
    for (i = 0; i < FAT_SIZE; i++) {
        if(fat1[i] == FAT_UNUSED){
            return i;
        }
    }
    return -1;
}

/**
 * Vrati pocet volnych clusteru
 *
 * */
int getEmptyClustersCount(int32_t fat1[FAT_SIZE]){
    int cnt = 0;
    int i = 0;
    for (i = 0; i < FAT_SIZE; i++) {
        if(fat1[i] == FAT_UNUSED){
            cnt++;
        }
    }
    return cnt;
}

/**
 * Vrati ukazatel na volne misto v adresari, pokud se adresar nevejde vraci NULL
 *
 * */
struct directory* getEmptyFolderSpace(char clusters[FAT_SIZE], struct directory* dir, int isRoot){
    struct directory *start;
    struct directory* tmp;
    if(isRoot){
        start = dir;
    }else{
        start = (struct directory *)(clusters + dir->start_cluster * CLUSTER_SIZE);
    }
    tmp = start;
    if(dir->isFile){
        start = (struct directory *)clusters;
        tmp = start;
    }
    while(strlen(start->name) > 0){
        start++;
    }
    if(start - tmp < 230){
        return start;
    }else {
        return NULL;
    }
}

/**
 * Vrati ukazatel na slozku nebo soubor se jmenem dirName,
 * pokud se slozka nebo soubor s timto nazvem nenachazi v adresari where,
 * vraci NULL
 *
 * */
struct directory* findDirectory(struct directory* where ,char* dirName){
    while(strlen(where->name) > 0){
        if(strcmp(where->name, dirName) == 0){
            return where;
        }
        where++;
    }
    return NULL;

}

/**
 * Vrati ukazatel na slozku nebo soubor definovany zadanou cestou path,
 * pokud je soubor nenalezen, vraci NULL
 *
 * */
struct directory* findDirectoryByPath(char clusters[FAT_SIZE], char* path){
    char *p = strtok(path, "/");
    struct directory* dir = (struct directory*)clusters;
    while (p != NULL)
    {
        dir = findDirectory(dir, p);
        p = strtok (NULL, "/");
        if(p == NULL){
            return dir;
        }
        if(dir == NULL){
            return NULL;
        }
        dir = (struct directory *)(clusters + dir->start_cluster * CLUSTER_SIZE);
    }
    return (struct directory*)clusters;
}

/**
 * Vytvori slozku se jmenem name v ceste path
 *
 * */
void createFolder(int32_t fat1[FAT_SIZE], int32_t fat2[FAT_SIZE],char clusters[FAT_SIZE],char* name,char* path){
    struct directory *dir;
    int isRoot = 0;
    int32_t clusterId;
    char fullPath[100];
    if(strlen(path) == 1){
        isRoot = 1;
    }
    memset(fullPath, '\0', sizeof(fullPath));
    strcpy(fullPath, path);
    fullPath[strlen(fullPath) - 1] = '/';
    strcpy(&fullPath[strlen(fullPath)],name);
    dir = findDirectoryByPath(clusters, fullPath);
    if(dir != NULL) {
        printf("FOLDER ALREADY EXISTS\n");
        return;
    }
    dir = findDirectoryByPath(clusters, path);
    if(dir == NULL){
        printf("PATH NOT FOUND\n");
        return;
    }else{
        dir = getEmptyFolderSpace(clusters, dir, isRoot);
        if(dir == NULL){
            printf("DIRECTORY IS FULL\n");
            return;
        }else{
            clusterId = getEmptyCluster(fat1);
            if(clusterId == -1){
                printf("DISC IS FULL\n");
                return;
            }else{
                struct directory* newDir = malloc(sizeof(struct directory));
                if(newDir == NULL){
                    printf("NOT ENOUGH MEMORY\n");
                    return;
                }else{
                    fat1[clusterId] = FAT_DIRECTORY;
                    newDir->start_cluster = clusterId;
                    newDir->isFile = 0;
                    strcpy(newDir->name, name);
                    newDir->size = 0;
                    memcpy(dir, newDir, sizeof(struct directory));
                    free(newDir);
                    printf("OK\n");
                    fat2[clusterId] = FAT_DIRECTORY;
                }
            }
        }
    }
}

/**
 * Pokud je slozka dir prazdna vraci true jinak false
 *
 * */
int isFolderEmpty(char clusters[FAT_SIZE],struct directory* dir){
    struct directory *start = (struct directory *)(clusters + dir->start_cluster * CLUSTER_SIZE);
    if(strlen(start->name) > 0){
        return 0;
    }else{
        return 1;
    }

}

/**
 * Odstrani zaznam slozky nebo souboru se jmenem name,
 * ze slozky dir a posune vsechny nasledovne zaznamy o jeden doleva
 *
 * */
void removeDirectoryFromFolder(struct directory* dir ,char* name){
    int found = 0;
    while(strlen(dir->name) > 0){
        if(strcmp(dir->name, name) == 0){
            found = 1;
        }
        if(found && strlen((dir+1)->name) > 0){
            memcpy(dir, dir + 1, sizeof(struct directory));
        }
        if(strlen((dir+1)->name) == 0){
            memset(dir, '\0', sizeof(struct directory));
        }
        dir++;
    }
}

/**
 * Odstrani slozku ktera se nachazi pod cestou fullPath
 *
 * */
void removeFolder(int32_t fat1[FAT_SIZE], int32_t fat2[FAT_SIZE],char clusters[FAT_SIZE],char* fullPath){
    struct directory *dir;
    char name[100];
    char path[100];
    char *p;
    int32_t clusterId;
    strcpy(path, fullPath);
    p = strtok(path, "/");
    while (p != NULL)
    {
        strcpy(name, p);
        p = strtok (NULL, "/");
    }
    strcpy(path, fullPath);
    path[strlen(path)-strlen(name)] = '\0';
    if(fullPath[strlen(fullPath)-1] != '/'){
        printf("PATH TO FOLDER MUST END WITH /\n");
        return;
    }
    dir = findDirectoryByPath(clusters, fullPath);
    if(dir == NULL){
        printf("PATH NOT FOUND\n");
        return;
    }else{
        if(!isFolderEmpty(clusters, dir)){
            printf("PATH NOT EMPTY\n");
            return;
        }else{
            clusterId = dir->start_cluster;
            fat1[clusterId] = FAT_UNUSED;
            removeDirectoryFromFolder(dir ,name);
            memset(clusters + clusterId * CLUSTER_SIZE, '\0', CLUSTER_SIZE);
            fat2[clusterId] = FAT_UNUSED;
            printf("OK\n");
        }
    }
}

/**
 * Ulozi soubor na ktery ukazuje fp do cesty fullPath
 *
 * */
void saveFile(int32_t fat1[FAT_SIZE], int32_t fat2[FAT_SIZE],char clusters[FAT_SIZE], FILE* fp,char* fullPath){
    struct directory* dir;
    char name[100];
    char path[100];
    char *p;
    int bytesRead;
    int fileSize;
    int isRoot = 0;
    int32_t cluster, prevCluster;
    strcpy(path, fullPath);
    if(fullPath[strlen(fullPath) - 1] == '/'){
        printf("PATH MUST END WITH FILE NAME\n");
        return;
    }
    dir = findDirectoryByPath(clusters, fullPath);
    if(dir != NULL) {
        printf("FILE ALREADY EXISTS\n");
        return;
    }
    p = strtok(path, "/");
    while (p != NULL)
    {
        strcpy(name, p);
        p = strtok (NULL, "/");
    }

    strcpy(path, fullPath);
    path[strlen(path)-strlen(name)] = '\0';
    if(strlen(path) == 1){
        isRoot = 1;
    }
    dir = findDirectoryByPath(clusters, path);
    if(dir == NULL){
        printf("PATH NOT FOUND\n");
        return;
    }else{
        fseek(fp, 0, SEEK_END);
        fileSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        if(getEmptyClustersCount(fat1) < ((int)ceil(fileSize/255.0))){
            printf("FILE IS TOO BIG\n");
            return;
        }else{
            dir = getEmptyFolderSpace(clusters, dir, isRoot);
            if(dir == NULL){
                printf("DIRECTORY IS FULL\n");
                return;
            }else {
                struct directory* newDir = malloc(sizeof(struct directory));
                if(newDir == NULL){
                    printf("NOT ENOUGH MEMORY\n");
                    return;
                }else{
                    cluster = getEmptyCluster(fat1);
                    fat1[cluster] = FAT_FILE_END;
                    newDir->start_cluster = cluster;
                    newDir->isFile = 1;
                    strcpy(newDir->name, name);
                    newDir->size = fileSize;
                    memcpy(dir, newDir, sizeof(struct directory));
                    free(newDir);

                    bytesRead = fread(clusters + cluster * CLUSTER_SIZE, 1, 255, fp);
                    *(clusters + cluster * CLUSTER_SIZE + bytesRead) = '\0';
                    fat2[cluster] = FAT_FILE_END;
                    while (bytesRead == 255) {
                        prevCluster = cluster;
                        cluster = getEmptyCluster(fat1);
                        fat1[cluster] = FAT_FILE_END;
                        fat1[prevCluster] = cluster;
                        bytesRead = fread(clusters + cluster * CLUSTER_SIZE, 1, 255, fp);
                        *(clusters + cluster * CLUSTER_SIZE + bytesRead) = '\0';
                        fat2[cluster] = FAT_FILE_END;
                        fat2[prevCluster] = cluster;
                    }
                    printf("OK\n");
                }
            }
        }
    }
}

/**
 * Odstrani soubor ktery se nachazi pod cestou fullPath
 *
 * */
void removeFile(int32_t fat1[FAT_SIZE], int32_t fat2[FAT_SIZE],char clusters[FAT_SIZE],char* fullPath){
    struct directory *dir;
    int i;
    char name[100];
    char path[100];
    char *p;
    int32_t clusterId;
    strcpy(path, fullPath);
    p = strtok(path, "/");
    while (p != NULL)
    {
        strcpy(name, p);
        p = strtok (NULL, "/");
    }
    strcpy(path, fullPath);
    path[strlen(path)-strlen(name)] = '\0';
    if(fullPath[strlen(fullPath) - 1] == '/'){
        printf("PATH MUST END WITH FILE NAME\n");
        return;
    }

    dir = findDirectoryByPath(clusters, fullPath);
    if(dir == NULL){
        printf("PATH NOT FOUND\n");
        return;
    }else{
        clusterId = dir->start_cluster;
        while (fat1[clusterId] != FAT_FILE_END && fat1[clusterId] != FAT_DIRECTORY){
            dir->start_cluster = fat1[clusterId];
            fat1[clusterId] = FAT_UNUSED;
            memset(clusters + clusterId * CLUSTER_SIZE, '\0', CLUSTER_SIZE);
            clusterId = dir->start_cluster;
        }
        memset(clusters + clusterId * CLUSTER_SIZE, '\0', CLUSTER_SIZE);
        fat1[clusterId] = FAT_UNUSED;
        removeDirectoryFromFolder(dir ,name);
        printf("OK\n");
        for(i = 0; i < FAT_SIZE ; i++){
            fat2[i] = fat1[i];
        }
    }
}

/**
 * Vytiskne na obrazovku vsechny clustery na kterych se nachazi soubor pod cestou path
 *
 * */
void printDirectoryClusters(int32_t fat1[FAT_SIZE], char clusters[FAT_SIZE],char* path){
    struct directory* dir = findDirectoryByPath(clusters, path);
    int clusterId;
    if(dir == NULL){
        printf("PATH NOT FOUND\n");
        return;
    }else {
        printf("%s %d", dir->name, dir->start_cluster);
        clusterId = dir->start_cluster;
        while (fat1[clusterId] != FAT_FILE_END && fat1[clusterId] != FAT_DIRECTORY) {
            clusterId = fat1[clusterId];
            printf(",%d",clusterId);
        }
        printf("\n");
    }
}

/**
 * Vypise obsah souboru ktery se nachazi pod cestou path
 *
 * */
void printDirectory(int32_t fat1[FAT_SIZE], char clusters[FAT_SIZE],char* path){
    struct directory* dir = findDirectoryByPath(clusters, path);
    int clusterId;
    if(dir == NULL){
        printf("PATH NOT FOUND\n");
        return;
    }else{
        printf("%s: ", dir->name);
        clusterId = dir->start_cluster;
        printf("%s", clusters + clusterId * CLUSTER_SIZE);
        while (fat1[clusterId] != FAT_FILE_END && fat1[clusterId] != FAT_DIRECTORY){
            clusterId = fat1[clusterId];

            printf("%s", clusters + clusterId * CLUSTER_SIZE);
        }
        printf("\r\n");

    }
}

/**
 * Vypise pomoc
 *
 * */
void printHelp(){
    printf("RUN WITH ARGUMENTS!\n");
    printf("First arg =\tpath to fat data structure(created if file does not exists)\n");
    printf("Second arg =\t[-a path from] saves file\n");
    printf("\t\t[-f path] removes file\n");
    printf("\t\t[-c path] print file clusters\n");
    printf("\t\t[-m name path] make folder\n");
    printf("\t\t[-r path] remove folder\n");
    printf("\t\t[-l path] print file contents\n");
    printf("\t\t[-p] print root tree\n");
}

/**
 * Pokud se v clusteru nachazi znacky FFFFFF vraci true jinak false
 *
 * */
int isClusterBad(char *cluster){
    int j;
    int rep = 0;
    char c;
        for (j = 0; j < CLUSTER_SIZE; j++) {
            c = *(cluster + j);
            if(c == 'F'){
                rep++;
            }else{
                rep = 0;
            }
            if(rep == 6){
                return 1;
            }
        }
    return 0;
}

/**
 * Datova struktura pro predani poli vlaknum
 *
 * */
struct dataArray{
    int32_t *fat1;
    int32_t *fat2;
    char *clusters;
};

/**
 * Pretizena metoda getEmptyCluster, vyuziva mutexy pro vicevlaknove operace
 *
 * */
int32_t getEmptyClusterMultithread(int32_t fat1[FAT_SIZE]){
    int i = 0;
    for (i = 0; i < FAT_SIZE; i++) {
        if (pthread_mutex_trylock(&mutexes[i]) == 0) {
            if(fat1[i] == FAT_UNUSED){
                return i;
            }
            pthread_mutex_unlock(&mutexes[i]);
        }
    }
    return -1;
}

/**
 * Zjisti a opravi zaznam ve slozce, pokud je oldClusterValue prvni cluster nejakeho souboru
 *
 * */
void repairPotentialDirectory(int32_t fat1[FAT_SIZE], char * clusters, int oldClusterValue, int newClusterValue){
    int i;
    struct directory *where;
    for (i = 0; i < FAT_SIZE; i++) {
        if(fat1[i] == FAT_DIRECTORY){
            where = (struct directory*)(clusters + i * CLUSTER_SIZE);
            while(strlen(where->name) > 0){
                if(where->start_cluster == oldClusterValue){
                    where->start_cluster = newClusterValue;
                }
                where++;
            }
        }
    }
}

/**
 * Projde cluster a nahradi znaky, které se nachazi mezi oznacenim FFFFFF mezerami,
 * pote nahraje upraveny obsah do prazdneho clusteru
 *
 * */
void repairCluster(int32_t fat1[FAT_SIZE],int32_t fat2[FAT_SIZE],char *clusters, int32_t badCluster){
    int32_t emptyCluster = getEmptyClusterMultithread(fat1);
    int i, rep = 0;
    char c, badChar;
    int bool = 0;
    if(emptyCluster == -1){
        printf("NOT ENOUGH SPACE TO REPAIR\n");
        return;
    }
    for(i = 0; i < FAT_SIZE ; i++){
        if(fat1[i] == badCluster){
            fat1[i] = emptyCluster;
            break;
        }
    }
    fat1[emptyCluster] = fat1[badCluster];
    for (i = 0; i < CLUSTER_SIZE; ++i) {
        badChar = *(clusters + badCluster * CLUSTER_SIZE + i);
        if(!bool){
            *(clusters + emptyCluster * CLUSTER_SIZE + i) = badChar;
        }else{
            *(clusters + emptyCluster * CLUSTER_SIZE + i) = ' ';
        }
        if(badChar == 'F'){
            rep++;
        }else{
            rep = 0;
        }
        if(rep > 5){
            bool = !bool;
            *(clusters + emptyCluster * CLUSTER_SIZE + i) = ' ';
            *(clusters + emptyCluster * CLUSTER_SIZE + i - 1) = ' ';
            *(clusters + emptyCluster * CLUSTER_SIZE + i - 2) = ' ';
            *(clusters + emptyCluster * CLUSTER_SIZE + i - 3) = ' ';
            *(clusters + emptyCluster * CLUSTER_SIZE + i - 4) = ' ';
            *(clusters + emptyCluster * CLUSTER_SIZE + i - 5) = ' ';
        }
    }
    fat1[badCluster] = FAT_BAD_CLUSTER;
    repairPotentialDirectory(fat1, clusters, badCluster, emptyCluster);
    pthread_mutex_unlock(&mutexes[emptyCluster]);
}

/**
 * Projde vsechny clustery a pokud jsou spatne tak je opravi
 *
 * */
void repairBadClusters(int32_t fat1[FAT_SIZE],int32_t fat2[FAT_SIZE],char *clusters){
    int i;
    int e;
    for (i = 0; i < FAT_SIZE; ++i) {
        if(fat1[i] != FAT_UNUSED && fat1[i] != FAT_DIRECTORY && fat1[i] != FAT_BAD_CLUSTER) {
            e = pthread_mutex_trylock(&mutexes[i]);
            if (e == 0) {
                if (isClusterBad(clusters + i * CLUSTER_SIZE)) {
                    repairCluster(fat1, fat2, clusters,i);
                }
                pthread_mutex_unlock(&mutexes[i]);
            }else{
                printf("%d Mutex error %d\n",i,e);
            }
        }
    }
}

/**
 * Spousteci procedura vlakna
 *
 * */
void *runThread(void *voidPtr){
    struct dataArray* da = (struct dataArray*)voidPtr;
    int32_t *fat1 = da->fat1;
    int32_t *fat2 = da->fat2;
    char *clusters = da->clusters;
    printf("Thread started\n");

    repairBadClusters(fat1, fat2, clusters);

    return 0;
}

/**
 * Inicializuje mutexy, vlakna, spusti a pote zpetne joine. Opravi celou FAT
 *
 * */
void repair(int32_t fat1[FAT_SIZE],int32_t fat2[FAT_SIZE],char clusters[FAT_SIZE],int threadCnt){
    pthread_t threads[threadCnt];
    int i = 0;
    int e = 0;
    struct dataArray* da = malloc(sizeof(struct dataArray));
    for(i = 0; i < FAT_SIZE ; i++){
       /* pthread_mutexattr_t pa;
        pthread_mutexattr_init(&pa);*/
        e = pthread_mutex_init(&mutexes[i], NULL);

        if(e != 0){
            printf("Mutex init error %d\n", e);
        }
    }
    da->clusters = clusters;
    da->fat1 = fat1;
    da->fat2 = fat2;
    for (i = 0; i < threadCnt; ++i) {
        e = pthread_create(&threads[i], NULL, runThread, da);
        printf("created thread %d -> %d\n", i+1, e);
    }

    for (i = 0; i < threadCnt; ++i) {
        e = pthread_join(threads[i], NULL);
        printf("joined thread %d -> %d\n", i+1,e);
    }
    free(da);
    printf("OK\n");
}

/**
 * Prida znacky FFFFFF do vsech datovych clusteru
 *
 * */
void corrupt(int32_t fat1[FAT_SIZE],char clusters[FAT_SIZE]){
    int i, j;
    for (i = 0; i < FAT_SIZE; ++i) {
        if(fat1[i] != FAT_DIRECTORY && fat1[i] != FAT_UNUSED && fat1[i] != FAT_BAD_CLUSTER){
            for (j = 10; j < 16; j++) {
                *(clusters + i * CLUSTER_SIZE + j) = 'F';
            }
            for (j = 150; j < 156; j++) {
                *(clusters + i * CLUSTER_SIZE + j) = 'F';
            }
        }
    }
    printf("OK\n");
}

/**
 * Inicializace a reakce na vstupni argumenty
 * po spusteni nacte FAT ze souboru do pameti
 * a po dokonceni zapise FAT zpet do souboru
 *
 * */
int main(int argc, char** argv) {
    int i,j;
    FILE *fp;
    int32_t *fat1 = malloc(sizeof(int32_t) * FAT_SIZE);
    int32_t *fat2 = malloc(sizeof(int32_t) * FAT_SIZE);

    char *clusters = malloc(sizeof(char) * FAT_SIZE * CLUSTER_SIZE);
    memset(clusters,'\0', sizeof(char) * FAT_SIZE * CLUSTER_SIZE);
    memset(fat1, '\0', sizeof(int32_t) * FAT_SIZE);
    memset(fat2, '\0', sizeof(int32_t) * FAT_SIZE);

    if(argc < 3){
        printHelp();
        return 0;
    }else {
        if ((fp = fopen(argv[1], "rb")) != NULL) {
            j = fread(fat1,1, sizeof(int32_t) * FAT_SIZE,  fp);
            j = fread(fat2, 1, sizeof(int32_t) * FAT_SIZE, fp);
            for (i = 0; i < FAT_SIZE; i++) {
                j = fread(clusters + i * CLUSTER_SIZE, 1,CLUSTER_SIZE, fp);
            }
            fclose(fp);
        } else {
            fat1[0] = FAT_DIRECTORY;
            fat2[0] = FAT_DIRECTORY;
            for (i = 1; i < FAT_SIZE; i++) {
                fat1[i] = FAT_UNUSED;
                fat2[i] = FAT_UNUSED;
            }
        }
    }

    if(!strcmp(argv[2], "-a")){
        if(argc == 5){
            fp = fopen(argv[4], "r");
            if(fp != NULL){
                saveFile(fat1, fat2, clusters,fp, argv[3]);
                fclose(fp);
            }else{
                printf("CAN'T OPEN FILE\n");
                return 0;
            }
        }else{
            printHelp();
            return 0;
        }
    }else if(!strcmp(argv[2], "-f")){
        if(argc == 4){
            removeFile(fat1, fat2, clusters, argv[3]);
        }else{
            printHelp();
            return 0;
        }
    }else if(!strcmp(argv[2], "-c")){
        if(argc == 4){
            printDirectoryClusters(fat1, clusters, argv[3]);
        }else{
            printHelp();
            return 0;
        }
    }else if(!strcmp(argv[2], "-m")){
        if(argc == 5){
            createFolder(fat1, fat2, clusters, argv[3], argv[4]);
        }else{
            printHelp();
            return 0;
        }
    }else if(!strcmp(argv[2], "-r")){
        if(argc == 4){
            removeFolder(fat1, fat2, clusters, argv[3]);
        }else{
            printHelp();
            return 0;
        }
    }else if(!strcmp(argv[2], "-l")){
        if(argc == 4){
            printDirectory(fat1, clusters, argv[3]);
        }else{
            printHelp();
            return 0;
        }
    }else if(!strcmp(argv[2], "-p")){
        printRoot(clusters);
    }else if(!strcmp(argv[2], "-repair")){
        if(argc == 4){
            clock_t timer = clock(), diff;
            repair(fat1, fat2,clusters, strtol(argv[3],NULL,10));
            diff = clock() - timer;
            double msec = (double)(diff * 1000) / CLOCKS_PER_SEC;
            printf("%f sec, %f milli", msec/1000, fmod(msec,1000));
        }else{
            printHelp();
            return 0;
        }
    }else if(!strcmp(argv[2], "-dump")){
        printAll(fat1, clusters);
    }else if(!strcmp(argv[2], "-corrupt")){
        corrupt(fat1, clusters);
    }else{
        printHelp();
        return 0;
    }
    fp = fopen(argv[1],"wb");
    fwrite(fat1, sizeof(int32_t) * FAT_SIZE, 1, fp);
    fwrite(fat2, sizeof(int32_t) * FAT_SIZE, 1, fp);
    for (i = 0; i < FAT_SIZE; i++) {
        fwrite(clusters + i * CLUSTER_SIZE, CLUSTER_SIZE, 1, fp);
    }
    /*
    fseeko(fp,-1,SEEK_END);
    off_t position = ftello(fp);
    ftruncate(fileno(fp), position);
     */
    fclose(fp);

    return 0;
}




