# Nhập các thư viện cần thiết
import numpy as np
import scipy.stats as stats

# Nhập số lượng mẫu, số lượng các giá trị phân biệt, giá trị tối thiểu và giá trị tối đa của mẫu
n = 20; #int(input("Nhập số lượng mẫu: "))
k = 200; #int(input("Nhập số lượng các giá trị phân biệt của mẫu: "))
min_value = 100; #float(input("Nhập giá trị tối thiểu của mẫu: "))
max_value = 3600; #float(input("Nhập giá trị tối đa của mẫu: "))

# Sinh ra ngẫu nhiên các giá trị của mẫu tuân theo phân bố chuẩn
# Bạn có thể thay đổi các tham số trung bình (mean) và độ lệch chuẩn (std) theo ý muốn
mean = (min_value + max_value) / 2 # Trung bình của phân bố chuẩn
std = (max_value - min_value) / 6 # Độ lệch chuẩn của phân bố chuẩn
sample = np.random.normal(mean, std, n) # Mẫu dữ liệu tuân theo phân bố chuẩn
sample = np.round(sample, 0)

# In ra mẫu dữ liệu
print("Mẫu dữ liệu tuân theo phân bố chuẩn là: ")
print(sample)

# Tính toán giá trị p_value để kiểm tra xem mẫu dữ liệu có phải là phân bố chuẩn hay không
# Sử dụng phương pháp Shapiro-Wilk, một trong những phương pháp phổ biến để kiểm định giả thuyết thống kê về phân bố chuẩn
# Bạn có thể tham khảo thêm về phương pháp này tại đây [^1^]
stat, p_value = stats.shapiro(sample) # Tính toán giá trị thống kê và giá trị p_value

# In ra giá trị p_value
print("Giá trị p_value là: ")
print(p_value)

# Đặt mức ý nghĩa thống kê (significance level) là 0.05
# Nếu giá trị p_value nhỏ hơn mức ý nghĩa thống kê, ta bác bỏ giả thuyết không và kết luận mẫu dữ liệu không tuân theo phân bố chuẩn
# Nếu giá trị p_value lớn hơn hoặc bằng mức ý nghĩa thống kê, ta không bác bỏ giả thuyết không và kết luận mẫu dữ liệu tuân theo phân bố chuẩn
alpha = 0.05 # Mức ý nghĩa thống kê
if p_value < alpha:
    print("Mẫu dữ liệu không tuân theo phân bố chuẩn")
else:
    print("Mẫu dữ liệu tuân theo phân bố chuẩn")

