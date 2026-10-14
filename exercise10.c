#include <stdio.h>
#include <string.h>

#define MAX_ZONES 10
#define MAX_QRS 20

typedef struct {
    int zone_id;
    char zone_name[30];
    double ticket_price;
    int total_capacity;
    int sold_tickets;
} TicketZone;

typedef struct {
    char qr_code[20];
    int zone_id;
    int is_used;
} QrCheckIn;

// Khai báo nguyên mẫu hàm
int find_zone_index(TicketZone zones[], int total_zones, int zone_id);
int process_ticket_purchase(TicketZone zones[], int total_zones, int zone_id, int quantity, int is_early_bird, double *final_amount);
int process_qr_checkin(QrCheckIn qrs[], int total_qrs, const char *qr_code);
void print_system_status(TicketZone zones[], int total_zones, QrCheckIn qrs[], int total_qrs);

int find_zone_index(TicketZone zones[], int total_zones, int zone_id) {
    if (total_zones <= 0) return -2;
    for (int i = 0; i < total_zones; i++) {
        if (zones[i].zone_id == zone_id) {
            return i;
        }
    }
    return -1;
}

int process_ticket_purchase(TicketZone zones[], int total_zones, int zone_id, int quantity, int is_early_bird, double *final_amount) {
    if (total_zones <= 0) return -5; // Lỗi truy xuất mảng rỗng
    if (quantity < 1 || quantity > 4) return -1; // Lỗi số lượng mua không hợp lệ

    int index = find_zone_index(zones, total_zones, zone_id);
    if (index == -1) return -3; // Lỗi mã khu vực không tồn tại

    TicketZone *target = &zones[index];
    if (target->sold_tickets + quantity > target->total_capacity) {
        return -2; // Lỗi tràn dung lượng khu vực (Overbooking)
    }

    double base_total = target->ticket_price * quantity;
    if (is_early_bird == 1) {
        *final_amount = base_total * 0.85; // Giảm 15%
    } else {
        *final_amount = base_total;
    }

    target->sold_tickets += quantity;
    return 0; // Thành công
}

int process_qr_checkin(QrCheckIn qrs[], int total_qrs, const char *qr_code) {
    if (total_qrs <= 0) return -5; // Lỗi truy xuất mảng rỗng
    
    for (int i = 0; i < total_qrs; i++) {
        if (strcmp(qrs[i].qr_code, qr_code) == 0) {
            if (qrs[i].is_used == 1) {
                return -4; // Lỗi trùng lặp check-in (Double Check-in)
            }
            qrs[i].is_used = 1;
            return 0; // Check-in thành công
        }
    }
    return -3; // Không tìm thấy mã QR
}

void print_system_status(TicketZone zones[], int total_zones, QrCheckIn qrs[], int total_qrs) {
    printf('\n--- TRANG THAI HE THONG TICKETBOX ---\n');
    printf('Danh sach Khu vực vé:\n');
    for (int i = 0; i < total_zones; i++) {
        printf('ID: %d | Ten: %s | Gia: %.2f | Suc chua: %d | Đã ban: %d\n',
               zones[i].zone_id, zones[i].zone_name, zones[i].ticket_price,
               zones[i].total_capacity, zones[i].sold_tickets);
    }
    printf('Danh sach Ma QR Check-in:\n');
    for (int i = 0; i < total_qrs; i++) {
        printf('QR: %s | Khu vực ID: %d | Trang thai: %s\n',
               qrs[i].qr_code, qrs[i].zone_id, qrs[i].is_used == 1 ? 'Đã sử dụng' : 'Chưa sử dụng');
    }
    printf('-------------------------------------\n');
}

int main() {
    TicketZone zones[MAX_ZONES] = {
        {101, 'VIP Lounge', 1500000.0, 5, 2},
        {102, 'Standard Zone', 500000.0, 10, 8}
    };
    int total_zones = 2;

    QrCheckIn qrs[MAX_QRS] = {
        {'QR_VIP_01', 101, 0},
        {'QR_STD_01', 102, 0}
    };
    int total_qrs = 2;

    printf('=== MO PHONG KIỂM THỬ HỆ THỐNG ===\n');
    print_system_status(zones, total_zones, qrs, total_qrs);

    double amount = 0.0;
    int result;

    // Test case 1: Mua vé hợp lệ có Early Bird
    result = process_ticket_purchase(zones, total_zones, 101, 2, 1, &amount);
    printf('Test 1 (Mua 2 vé VIP Early Bird): Ket qua = %d, So tien = %.2f\n', result, amount);

    // Test case 2: Mua vé vượt quá sức chứa (Overbooking)
    result = process_ticket_purchase(zones, total_zones, 102, 5, 0, &amount);
    printf('Test 2 (Mua 5 vé Standard - Tràn kho): Ket qua = %d\n', result);

    // Test case 3: Check-in QR lần đầu thành công
    result = process_qr_checkin(qrs, total_qrs, 'QR_VIP_01');
    printf('Test 3 (Quét QR lần 1): Ket qua = %d\n', result);

    // Test case 4: Check-in QR trùng lặp (Double Check-in)
    result = process_qr_checkin(qrs, total_qrs, 'QR_VIP_01');
    printf('Test 4 (Quét QR lần 2 - Trùng lặp): Ket qua = %d\n', result);

    print_system_status(zones, total_zones, qrs, total_qrs);
    return 0;
}
