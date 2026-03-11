### \# Linux Syslog Analiz ve Bağlı Liste Entegrasyonu



Bu proje, Veri Yapıları ve Algoritmalar dersi kapsamında geliştirilmiş olup, Linux işletim sistemindeki sistem günlüklerinin (syslog) C dili kullanılarak ayrıştırılması (parsing) ve dinamik bellek yönetimiyle bellekte tutulması amacıyla tasarlanmıştır.



##### \## 📌 Projenin Amacı

İşletim sistemleri, donanım veya arka plan servisleri (daemon) tarafından üretilen log kayıtlarını genellikle `/var/log/syslog` gibi dosyalarda ardışık olarak tutar. Bu projenin temel amacı, durağan bir metin dosyasındaki bu log satırlarını okuyarak, bellek üzerinde çalışma zamanında (runtime) büyüyüp küçülebilen yapısal bir formata dönüştürmek ve bu veriler üzerinde performanslı okuma işlemleri gerçekleştirmektir.



##### \## ⚙️ Kullanılan Veri Yapısı ve Tercih Sebebi

Bu projede \*\*Çift Yönlü Bağlı Liste (Doubly Linked List)\*\* veri yapısı kullanılmıştır.



##### \*\*Neden Çift Yönlü Bağlı Liste?\*\*

1\. \*\*Kronolojik Yapı:\*\* Syslog dosyaları eskiden yeniye doğru sırayla yazılır. Sistem yöneticileri ise genellikle hata ayıklama (debugging) yaparken \*\*en son gerçekleşen olayları (en güncel logları)\*\* görmek isterler.

2\. \*\*Performanslı Geri Okuma:\*\* Tek yönlü bağlı listelerde (Singly Linked List) son elemandan geriye doğru okuma yapmak yüksek algoritmik maliyet gerektirir. Ancak Çift Yönlü Bağlı Liste'de bulunan `prev` (önceki) işaretçileri sayesinde, dosyanın sonundaki en güncel loglardan başlayarak geriye doğru (backward traversal) O(n) zaman karmaşıklığıyla, son derece hızlı ve esnek bir şekilde okuma yapılabilmektedir.

##### 

##### \## 🚀 Proje Özellikleri

\* \*\*Dinamik Ayrıştırma (Parsing):\*\* `sscanf` kullanılarak standart log satırları (Tarih, Host Adı, Servis Adı, Mesaj) yapısal (struct) parçalara ayrılır.

\* \*\*Dinamik Bellek Yönetimi:\*\* Her yeni log kaydı için `malloc` ile heap bellekten alan tahsis edilir.

\* \*\*İki Yönlü Okuma:\*\* Loglar hem baştan sona (eskiden yeniye) hem de sondan başa (yeniden eskiye) doğru listelenebilir.

\* \*\*Güvenli Bellek Temizliği:\*\* Program sonlanırken oluşturulan tüm düğümler `free` fonksiyonu ile işletim sistemine iade edilerek \*Memory Leak (Bellek Sızıntısı)\* engellenir.

##### 

##### \## 🛠️ Kurulum ve Çalıştırma



Projeyi kendi bilgisayarınızda çalıştırmak için aşağıdaki adımları izleyebilirsiniz:



1\. Repoyu bilgisayarınıza klonlayın:

&#x20;  ```bash

&#x20;  git clone \[https://github.com/KULLANICI\_ADINIZ/linux-syslog-linked-list.git](https://github.com/KULLANICI\_ADINIZ/linux-syslog-linked-list.git)

##### 

##### 💻 Örnek Çıktı

Uygulama çalıştığında konsol ekranında önce logların kronolojik sırası, ardından çift yönlü listenin avantajını gösteren sondan başa doğru (en güncelden eskiye) sıralanmış hali gösterilir:

\--- LOGLAR (ILERI DOGRU / KRONOLOJIK) ---

\[Mar 11 08:30:01] Islam-PC kernel: -> \[ 0.000000] Linux version 5.4.0 started

\[Mar 11 09:15:32] Islam-PC sshd\[1234]: -> Accepted password for root from 192.168.1.10



\--- LOGLAR (GERIYE DOGRU / EN GUNCELDEN ESKIYE) ---

\[Mar 11 09:15:32] Islam-PC sshd\[1234]: -> Accepted password for root from 192.168.1.10

\[Mar 11 08:30:01] Islam-PC kernel: -> \[ 0.000000] Linux version 5.4.0 started



Bellek basariyla sisteme iade edildi.

