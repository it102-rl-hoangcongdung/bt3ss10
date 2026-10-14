# BÁO CÁO BÀI TẬP 03: PHÂN TÍCH TRADE-OFF TRUYỀN THAM TRỊ VS CON TRỎ VÀ TÁI CẤU TRÚC HỆ THỐNG VÉ SỰ KIỆN

> 👤 **Học viên:** Hoàng Công Dũng | **Mã SV:** PTIT-HCM-009
> 🏫 **Môn học:** it102-rl-hoangcongdung

---

## 📊 Sơ đồ thiết kế hệ thống (Activity Diagram)

> 💡 *Sơ đồ dưới đây được render tự động trực tiếp trên GitHub bằng Mermaid. Bạn cũng có thể tải file **`bt3.drawio`** trong repository này để mở và chỉnh sửa trực tiếp trên [Draw.io (diagrams.net)](https://app.diagrams.net).* 

```mermaid
graph TD
  Start([Bắt đầu]) --> InputChoice{Chọn chức năng}</
  InputChoice -- 1. Mua vé --> CheckZone[Kiểm tra tồn tại khu vực và dung lượng]
  CheckZone --> ValidZone{Khu vực hợp lệ & Còn chỗ?}
  ValidZone -- Không --> ErrorZone[Báo lỗi: Tràn kho hoặc Không tồn tại] --> EndFail([Kết thúc: Thất bại])
  ValidZone -- Có --> CheckQty{Số lượng vé 1 <= qty <= 4?}
  CheckQty -- Không --> ErrorQty[Báo lỗi: Vượt hạn mức mua] --> EndFail
  CheckQty -- Có --> CalcPrice[Tính tiền: Áp dụng Early Bird nếu có]
  CalcPrice --> UpdateStock[Cập nhật sold_tickets = sold_tickets + quantity]
  UpdateStock --> EndSuccess([Kết thúc: Bán vé thành công])
  InputChoice -- 2. Check-in QR --> CheckQR[Tìm mã QR trong danh sách]
  CheckQR --> ValidQR{Mã QR tồn tại?}
  ValidQR -- Không --> ErrorQR[Báo lỗi: Mã QR không hợp lệ] --> EndFail
  ValidQR -- Có --> CheckStatus{Trạng thái is_used == 0?}
  CheckStatus -- Đã dùng (1) --> ErrorUsed[Cảnh báo: Vé đã được sử dụng] --> EndFail
  CheckStatus -- Chưa dùng (0) --> SetUsed[Cập nhật is_used = 1]
  SetUsed --> EndCheckIn([Kết thúc: Check-in thành công])
```

---

## Nhiệm vụ 1: Phân tích Đầu vào, Đầu ra và Đề xuất 2 Giải pháp Kiến trúc

Trong bài toán quản lý bán vé sự kiện Ticketbox Live Concert, việc gom toàn bộ logic từ tính tiền, kiểm tra tồn kho đến quét mã QR vào hàm main hoặc một hàm khổng lồ gây ra hiện tượng vi phạm nguyên tắc Đơn trách nhiệm (Single Responsibility Principle).

Để giải quyết triệt để vấn đề này, chúng ta phân tích các tham số đầu vào và đầu ra cho từng module hạt nhân:

- Module Bán vé: Đầu vào gồm mảng cấu trúc TicketZone, số lượng khu vực, mã zone_id người dùng chọn, số lượng vé mua (quantity), và cờ Early Bird. Đầu ra trả về mã lỗi dạng số nguyên (0 nếu thành công, các mã âm tương ứng với lỗi cụ thể như quá hạn mức, tràn kho, không tìm thấy khu vực) và cập nhật trực tiếp biến sold_tickets qua con trỏ.
- Module Check-in QR: Đầu vào gồm mảng cấu trúc QrCheckIn, tổng số lượng mã QR, và mã QR cần quét. Đầu ra trả về trạng thái quét thành công hay thất bại, đồng thời thay đổi trực tiếp trạng thái is_used từ 0 lên 1.

## Nhiệm vụ 2: Đánh giá Trade-off giữa Truyền Tham trị và Truyền Con trỏ

Đứng ở góc độ kỹ thuật lập trình C, chúng ta có 2 phương án thiết kế kiến trúc hàm để truyền dữ liệu cấu trúc:

| Tiêu chí so sánh | Giải pháp 1: Truyền Tham trị (Pass by Value) | Giải pháp 2: Truyền Con trỏ / Địa chỉ (Pass by Pointer) |
| --- | --- | --- |
| Tính đóng gói (Encapsulation) | Kém vì dữ liệu truyền vào chỉ là bản sao, không phản ánh thay đổi thực tế vào cơ sở dữ liệu gốc. | Tốt, hàm thao tác trực tiếp trên vùng nhớ gốc của thực thể cấu trúc. |
| Khả năng tái sử dụng (Reusability) | Thấp, khó xây dựng các hàm cập nhật trạng thái kho hay trạng thái vé dùng chung. | Cao, các hàm xử lý được viết độc lập, nhận địa chỉ mảng cấu trúc để tương tác linh hoạt. |
| Kiểm soát lỗi biên (Edge-case Handling) | Khó khăn trong việc trả về đồng thời kết quả thực thi và thông báo lỗi qua return. | Tối ưu, dùng giá trị trả về của hàm cho mã lỗi (status code) và dùng con trỏ để thay đổi dữ liệu. |
| Độ phức tạp bảo trì (Maintainability) | Phức tạp khi hệ thống phình to do phải sao chép toàn bộ struct gây tốn bộ nhớ stack. | Gọn gàng, tối ưu hiệu năng bộ nhớ, dễ dàng scale hệ thống và viết unit test kiểm thử. |

## Nhiệm vụ 3: Thiết kế Nguyên mẫu hàm và Mã nguồn C hoàn chỉnh

Lựa chọn Giải pháp 2 (Truyền con trỏ và địa chỉ ô nhớ) làm phương án tối ưu thực thi cho bài tập. Dưới đây là mã nguồn C hoàn chỉnh được viết theo đúng chuẩn yêu cầu, biên giới an toàn và xử lý đầy đủ các bẫy dữ liệu nghiệp vụ.

- Áp dụng nguyên mẫu hàm tường minh cho từng tác vụ tìm kiếm, kiểm tra và cập nhật.
- Xử lý triệt để 5 nhóm lỗi thường gặp gồm: số lượng mua không hợp lệ, overbooking khu vực, mã khu vực không tồn tại, trùng lặp check-in và truy xuất mảng rỗng.

---

## 📁 Danh sách tệp tin nộp bài trong Repository
- 📝 `bt3.docx`: Báo cáo tài liệu phân tích nghiệp vụ hoàn chỉnh.
- 🎨 `bt3.drawio`: File thiết kế sơ đồ chuẩn theo quy định đề bài (mở trực tiếp bằng [Draw.io](https://app.diagrams.net) hoặc Lucidchart).
- 💻 `exercise10.c`: Mã nguồn chương trình.
