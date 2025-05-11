#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define WIFI_SSID "moto g73 5G_3103"
#define WIFI_PASSWORD "meunome9988"
#define THINGSPEAK_HOST "api.thingspeak.com"
#define THINGSPEAK_PORT 80
#define API_KEY "YGNH9NOKJ0X0D7HP"
#define BTN_A_PIN 5
#define BTN_B_PIN 6
#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define DHT_PIN 8

// Globais
int btn_a_state, btn_b_state;
int x_position, y_position;
float sensor_data = 0.0; // umidade
float temperatura = 0.0; // temperatura do DHT11
char wind_rose_direction[20] = "Centro";

// ---------- Funções Auxiliares ----------
void read_buttons() {
btn_a_state = !gpio_get(BTN_A_PIN);
btn_b_state = !gpio_get(BTN_B_PIN);
}

void calculate_wind_direction(int x, int y) {
    const int center = 2048;
    const int threshold = 700;

    int dx = x - center;
    int dy = y - center;

    if (abs(dx) < threshold && abs(dy) < threshold) {
        strcpy(wind_rose_direction, "Centro");
        return;
    }

    if (abs(dx) > threshold && abs(dy) > threshold) {
        if (dy > 0 && dx > 0) strcpy(wind_rose_direction, "Nordeste");
        else if (dy > 0 && dx < 0) strcpy(wind_rose_direction, "Noroeste");
        else if (dy < 0 && dx > 0) strcpy(wind_rose_direction, "Sudeste");
        else if (dy < 0 && dx < 0) strcpy(wind_rose_direction, "Sudoeste");
    } else if (abs(dy) > abs(dx)) {
        if (dy > 0) strcpy(wind_rose_direction, "Norte");
        else strcpy(wind_rose_direction, "Sul");
    } else {
        if (dx > 0) strcpy(wind_rose_direction, "Leste");
        else strcpy(wind_rose_direction, "Oeste");
    }
}


int direction_to_number(const char *dir) {
    if (strcmp(dir, "Centro") == 0) return 0;
    if (strcmp(dir, "Norte") == 0) return 1;
    if (strcmp(dir, "Nordeste") == 0) return 2;
    if (strcmp(dir, "Leste") == 0) return 3;
    if (strcmp(dir, "Sudeste") == 0) return 4;
    if (strcmp(dir, "Sul") == 0) return 5;
    if (strcmp(dir, "Sudoeste") == 0) return 6;
    if (strcmp(dir, "Oeste") == 0) return 7;
    if (strcmp(dir, "Noroeste") == 0) return 8;
    return -1; 
}


bool read_dht_sensor(uint8_t *temperature, uint8_t *humidity) {
uint8_t data[5] = {0};
int i, j;
gpio_set_dir(DHT_PIN, GPIO_OUT);
gpio_put(DHT_PIN, 0);
sleep_ms(18);
gpio_put(DHT_PIN, 1);
sleep_us(40);
gpio_set_dir(DHT_PIN, GPIO_IN);

uint32_t timeout = 10000;
while (gpio_get(DHT_PIN) == 0 && timeout--);
if (timeout == 0) return false;

timeout = 10000;
while (gpio_get(DHT_PIN) == 1 && timeout--);
if (timeout == 0) return false;

for (i = 0; i < 5; i++) {
    for (j = 7; j >= 0; j--) {
        timeout = 10000;
        while (gpio_get(DHT_PIN) == 0 && timeout--);
        if (timeout == 0) return false;

        sleep_us(30);
        if (gpio_get(DHT_PIN)) {
            data[i] |= (1 << j);
        }

        timeout = 10000;
        while (gpio_get(DHT_PIN) == 1 && timeout--);
        if (timeout == 0) return false;
    }
}

if ((data[0] + data[1] + data[2] + data[3]) != data[4]) {
    return false;
}

*humidity = data[0];
*temperature = data[2];
return true;
}

void read_analog() {
adc_select_input(0);
x_position = adc_read();
adc_select_input(1);
y_position = adc_read();
calculate_wind_direction(x_position, y_position);
uint8_t temp = 0, hum = 0;
if (read_dht_sensor(&temp, &hum)) {
    temperatura = (float)temp;
    sensor_data = (float)hum;
} else {
    printf("Erro ao ler sensor DHT11\n");
    temperatura = 0.0;
    sensor_data = 0.0;
}
}

// ---------- HTTP Request ----------
static ip_addr_t server_ip;
static volatile bool dns_done = false;

void dns_callback(const char *name, const ip_addr_t *ipaddr, void *arg) {
if (ipaddr) server_ip = *ipaddr;
dns_done = true;
}

bool resolve_dns() {
dns_done = false;
err_t err = dns_gethostbyname(THINGSPEAK_HOST, &server_ip, dns_callback, NULL);
if (err == ERR_OK) return true;
absolute_time_t timeout = make_timeout_time_ms(10000);
while (!dns_done && absolute_time_diff_us(get_absolute_time(), timeout) > 0) {
    cyw43_arch_poll();
    sleep_ms(100);
}
return dns_done;
}

bool send_data() {
struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
if (!pcb) return false;
if (tcp_connect(pcb, &server_ip, THINGSPEAK_PORT, NULL) != ERR_OK) {
    tcp_close(pcb);
    return false;
}

char request[512];
int wind_dir_num = direction_to_number(wind_rose_direction);

snprintf(request, sizeof(request),
    "GET /update?api_key=%s"
    "&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&field6=%.2f&field7=%.2f HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Connection: close\r\n\r\n",
    API_KEY,
    btn_a_state,
    btn_b_state,
    x_position,
    y_position,
    wind_dir_num,
    sensor_data,
    temperatura,
    THINGSPEAK_HOST);

printf("Pacote HTTP enviado:\n%s\n", request);

err_t err = tcp_write(pcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
if (err != ERR_OK) {
    tcp_close(pcb);
    return false;
}

tcp_output(pcb);
sleep_ms(1000);
tcp_close(pcb);
return true;
}

// ---------- MAIN ----------
int main() {
stdio_init_all();
gpio_init(BTN_A_PIN); gpio_set_dir(BTN_A_PIN, GPIO_IN); gpio_pull_up(BTN_A_PIN);
gpio_init(BTN_B_PIN); gpio_set_dir(BTN_B_PIN, GPIO_IN); gpio_pull_up(BTN_B_PIN);
adc_init();
adc_gpio_init(JOYSTICK_X_PIN);
adc_gpio_init(JOYSTICK_Y_PIN);
gpio_init(DHT_PIN);
gpio_set_dir(DHT_PIN, GPIO_OUT);
gpio_put(DHT_PIN, 1);
if (cyw43_arch_init()) return 1;
cyw43_arch_enable_sta_mode();

printf("Conectando-se ao Wi-Fi...\n");
if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
    printf("Erro ao conectar no Wi-Fi\n");
    return 1;
}
printf("Wi-Fi conectado!\n");

while (true) {
    read_buttons();
    read_analog();

    if (resolve_dns()) {
        bool ok = send_data();
        printf(ok ? "Dados enviados!\n" : "Erro ao enviar dados\n");
    } else {
        printf("Erro ao resolver DNS\n");
    }

    sleep_ms(15000); // Intervalo mínimo exigido pela ThingSpeak
}

    return 0;
}