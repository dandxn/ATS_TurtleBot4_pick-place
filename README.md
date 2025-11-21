# Lokalisasi, Pemetaan, dan Navigasi Otonom Menggunakan ROS2
1. Installation & Setup (Mengikuti ROS2 Humble + Struktur Proyek Anda)
1.1. Instalasi ROS2 Humble di Ubuntu 22.04
   sudo apt update && sudo apt install locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

sudo apt install software-properties-common
sudo add-apt-repository universe
sudo apt update
sudo apt install curl -y
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg

echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list

sudo apt update && sudo apt upgrade -y
sudo apt install ros-humble-desktop
sudo apt install ros-dev-tools

echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc
source ~/.bashrc

2. Membuat Workspace & Menambahkan Paket klppoint
   membuat workspace :
   mkdir -p klp1/src
   cd klp1/src
   cd ..
   cd ~/klp1_ws
   colcon build
   source install/setup.bash
   echo "source ~/klp1_ws/install/setup.bash" >> ~/.bashrc

   
  Struktur proyek:
  
  klp1/
  └── src/
        └── klppoint/
              ├── launch/
              ├── maps/
              ├── config/
              └── src/klppoint.cpp

3. Struktur Direktori
   
   launch/
berisi:
-navigation.launch.py
-localization.launch.py
-uts_nav.launch.py

maps/
berisi:
-antonajo.pgm
-klp1ajo.pgm
-kelompok1amalam.pgm

config/
localizaztion.yaml
nav2_params.yaml

src/
berisi:
klppoint.cpp (node navigasi + beep)

CMakeLists.txt
Paket Anda sudah sesuai dan sudah lengkap untuk diinstall.
Pada CMakeLists.txt edit, lalu tambahkan di atas if(BUILD_TESTING):
“install(
  DIRECTORY launch config maps
  DESTINATION share/${PROJECT_NAME}
)”

4. Menjalankan Lokalisasi, Navigasi, dan Node Pengantar Barang
   
-localization.launch.py → menjalankan AMCL + map server
-navigation.launch.py → menjalankan Nav2 stack
-uts_nav.launch.py → launch gabungan (bawa Map + Nav2)
-klppoint (node C++) → mengirim 2 goal otomatis + beep

Sebelum Menjalankan: Akses Robot via SSH

Jika robot fisik dipakai:
ssh ubuntu@192.168.xxx.xxx
alamat:
ssh ubuntu@192.168.185.3

Menjalankan Sistem
Terminal 1 – Jalankan Node Navigasi Otomatis
cd ~/klp1
ros2 run klppoint klppoint

Node ini:
-Mengirim Goal 1 → lokasi PICK
-Memutar 1 beep
-Mengirim Goal 2 → lokasi DROP
-Memutar 2 beep

Terminal 2 – Jalankan Lokalisasi
Menggunakan map .pgm
cd ~/klp1_ws
ros2 launch klppoint localization.launch.py map:=install/klppoint/share/klppoint/maps/antonajo.yaml

Terminal 3 – Jalankan Navigasi Nav2
cd ~/klp1
ros2 launch klppoint uts_nav.launch.py

uts_nav.launch.py dalam paket Anda sudah menggabungkan:
map server
localization
nav2_bringup
parameter YAML

5. Lokalisasi, Pemetaan, dan Navigasi
Lokalisasi

-Menggunakan AMCL di localization.launch.py
-Memanfaatkan peta .pgm yang sudah tersedia di folder maps/
-Pose robot dibaca dari /amcl_pose

Pemetaan

-Pada proyek ini peta sudah tersedia, sehingga robot tidak menjalankan SLAM lagi.
-File peta: antonajo.pgm, klp1ajo.pgm, kelompok1amalam.pgm

Navigasi Otonom

-Menggunakan Nav2 (ROS2)
-Stack dijalankan oleh navigation.launch.py
-Goal dikirim otomatis oleh node klppoint.cpp
-Beep 1x di goal pertama (pickup)
-Beep 2x di goal kedua (drop)

6. Analisis Hasil
    1. Keberhasilan Navigasi
-Apakah robot sukses mencapai Goal 1 dan Goal 2?

2. Waktu tempuh
-Waktu dari start → pick → drop

3. Validasi Lokalisasi
-Stabilitas /amcl_pose-
-Error drift minimal

4. Jalur Navigasi
-Visualisasi di RVIZ2 menggunakan /plan
-Apakah robot menghindari obstacle?

5. Verifikasi Buzzer
-Saat Goal 1 tercapai: beep 1x
-Saat Goal 2 tercapai: beep 2x








