from pdf2jpg import pdf2jpg


if __name__ == '__main__':
    # nhập tên tệp pdf
    input_path = input("Nhập tên tệp pdf: ")
    # nhập tên thư mục lưu ảnh
    output_path = input("Nhập tên thư mục lưu ảnh: ")
    


    # Thiết lập các tham số
    pages = "ALL"
    dpi = 300
    quality = 100
    convert_all_pages = True

    # Chuyển đổi tệp PDF thành các tệp ảnh JPG
    result = pdf2jpg.convert_pdf2jpg(input_path, output_path)