#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * SyslogNode: Cift Yonlu Bagli Liste (Doubly Linked List) dugum yapisi.
 * Her bir dugum, syslog dosyasindaki bir satiri (log kaydini) temsil eder.
 */
typedef struct SyslogNode {
    char timestamp[64];  // Logun tarih ve saat bilgisi
    char hostname[64];   // Logu ureten host (makine) adi
    char process[64];    // Logu ureten servis veya surec
    char message[512];   // Logun asil icerik/hata mesaji
    
    struct SyslogNode* prev; // Bir onceki dugumu isaret eder
    struct SyslogNode* next; // Bir sonraki dugumu isaret eder
} SyslogNode;

/*
 * Fonksiyon: create_node
 * Amaci: Parametre olarak gelen ham log satirini ayristirir (parse eder),
 * dinamik bellekten alan ayirir ve yeni bir dugum olusturur.
 */
SyslogNode* create_node(const char* raw_log_line) {
    SyslogNode* new_node = (SyslogNode*)malloc(sizeof(SyslogNode));
    
    if (new_node == NULL) {
        printf("Hata: Bellek tahsis edilemedi!\n");
        exit(1);
    }
    
    new_node->prev = NULL;
    new_node->next = NULL;
    
    char month[16], day[16], time[32];
    
    // sscanf ile log satirini anlamli parcalara boluyoruz
    // %[^\n] formati, satir sonuna kadar olan kismi tek parca olarak alir
    int parsed_items = sscanf(raw_log_line, "%s %s %s %s %s %[^\n]", 
                              month, day, time, new_node->hostname, new_node->process, new_node->message);
    
    // Tarih bilesenlerini birlestiriyoruz
    snprintf(new_node->timestamp, sizeof(new_node->timestamp), "%s %s %s", month, day, time);
    
    // Format standart disiysa ham satiri direkt mesaja aliyoruz
    if (parsed_items < 6) {
        strcpy(new_node->timestamp, "Bilinmiyor");
        strcpy(new_node->hostname, "Bilinmiyor");
        strcpy(new_node->process, "Bilinmiyor");
        strncpy(new_node->message, raw_log_line, sizeof(new_node->message) - 1);
    }
    
    return new_node;
}

/*
 * Fonksiyon: append_log
 * Amaci: Olusturulan log dugumunu cift yonlu bagli listenin sonuna ekler. O(1) karmasikligindadir.
 */
void append_log(SyslogNode** head, SyslogNode** tail, const char* raw_log_line) {
    SyslogNode* new_node = create_node(raw_log_line);
    
    // Liste bossa ilk dugumu ekle
    if (*head == NULL) {
        *head = new_node;
        *tail = new_node;
    } else {
        // Liste doluysa son dugumun (tail) arkasina ekle ve baglari kur
        (*tail)->next = new_node; 
        new_node->prev = *tail;   
        *tail = new_node;         
    }
}

/*
 * Fonksiyon: print_logs_forward
 * Amaci: Loglari bastan sona (eskiden yeniye) kronolojik olarak ekrana yazdirir.
 */
void print_logs_forward(SyslogNode* head) {
    printf("\n--- LOGLAR (ILERI DOGRU / KRONOLOJIK) ---\n");
    SyslogNode* current = head;
    
    while (current != NULL) {
        printf("[%s] %s %s -> %s\n", current->timestamp, current->hostname, current->process, current->message);
        current = current->next; 
    }
}

/*
 * Fonksiyon: print_logs_reverse
 * Amaci: Loglari sondan basa (yeniden eskiye) yazdirir. 
 * Bu islem cift yonlu bagli listenin asil kullanilma amacidir (guncel hatalari bulmak).
 */
void print_logs_reverse(SyslogNode* tail) {
    printf("\n--- LOGLAR (GERIYE DOGRU / EN GUNCELDEN ESKIYE) ---\n");
    SyslogNode* current = tail;
    
    while (current != NULL) {
        printf("[%s] %s %s -> %s\n", current->timestamp, current->hostname, current->process, current->message);
        current = current->prev; // prev isaretcisi ile geriye dogru gezinme (backward traversal)
    }
}

/*
 * Fonksiyon: free_memory
 * Amaci: Program sonlanirken malloc ile heap'ten alinan bellek alanlarini iade eder (Memory leak onler).
 */
void free_memory(SyslogNode* head) {
    SyslogNode* current = head;
    SyslogNode* next_node;
    
    while (current != NULL) {
        next_node = current->next; // Dangling pointer olmamasi icin adresi yedekle
        free(current);             // Mevcut dugumu bellekten temizle
        current = next_node;       // Sonraki dugume gec
    }
    printf("\nBellek basariyla sisteme iade edildi.\n");
}

int main() {
    // Bagli liste isaretcileri (Guvenlik icin NULL ile baslatilir)
    SyslogNode* head = NULL;
    SyslogNode* tail = NULL;
    
    // Okunacak log dosyasi (Proje dizininde bulunmalidir)
    const char* filename = "test_syslog.txt";
    FILE* file = fopen(filename, "r");
    
    if (file == NULL) {
        printf("Hata: '%s' dosyasi acilamadi!\nLutfen proje klasorunde bu dosyayi olusturun.\n", filename);
        return 1;
    }
    
    char line_buffer[1024]; 
    
    // Dosya sonuna (EOF) kadar satir satir okuma islemi
    while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
        append_log(&head, &tail, line_buffer); // Okunan satiri listeye ekle
    }
    
    fclose(file); 
    
    // Gelistirilen fonksiyonlarin test edilmesi
    print_logs_forward(head);
    print_logs_reverse(tail);
    
    // Bellek temizligi
    free_memory(head);
    
    return 0;
}
