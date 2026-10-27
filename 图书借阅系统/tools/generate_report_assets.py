from __future__ import annotations

from pathlib import Path
from typing import Iterable

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
ASSET_DIR = ROOT / "report" / "assets"
FONT_REGULAR = Path(r"C:\Windows\Fonts\msyh.ttc")
FONT_BOLD = Path(r"C:\Windows\Fonts\msyhbd.ttc")
FONT_MONO = Path(r"C:\Windows\Fonts\consola.ttf")


def font(size: int, bold: bool = False) -> ImageFont.FreeTypeFont:
    path = FONT_BOLD if bold else FONT_REGULAR
    return ImageFont.truetype(str(path), size)


def text_size(draw: ImageDraw.ImageDraw, text: str,
              selected_font: ImageFont.FreeTypeFont) -> tuple[int, int]:
    box = draw.textbbox((0, 0), text, font=selected_font)
    return box[2] - box[0], box[3] - box[1]


def wrap_text(draw: ImageDraw.ImageDraw, text: str,
              selected_font: ImageFont.FreeTypeFont, max_width: int) -> list[str]:
    lines: list[str] = []
    for paragraph in text.splitlines() or [""]:
        current = ""
        for char in paragraph:
            trial = current + char
            if current and text_size(draw, trial, selected_font)[0] > max_width:
                lines.append(current)
                current = char
            else:
                current = trial
        lines.append(current)
    return lines


def draw_centered_lines(draw: ImageDraw.ImageDraw, box: tuple[int, int, int, int],
                        lines: Iterable[str], selected_font: ImageFont.FreeTypeFont,
                        fill: str = "#111827") -> None:
    items = list(lines)
    line_height = selected_font.size + 8
    total_height = line_height * len(items)
    x0, y0, x1, _ = box
    y = y0 + ((box[3] - box[1]) - total_height) // 2
    for line in items:
        width, _ = text_size(draw, line, selected_font)
        draw.text((x0 + ((x1 - x0) - width) // 2, y),
                  line, font=selected_font, fill=fill)
        y += line_height


def draw_box(draw: ImageDraw.ImageDraw, box: tuple[int, int, int, int],
             title: str, attributes: list[str], methods: list[str]) -> None:
    x0, y0, x1, y1 = box
    title_height = 48
    line_gap = 31
    attr_start = y0 + title_height + 12
    method_start = attr_start + len(attributes) * line_gap + 12

    draw.rounded_rectangle(box, radius=10, fill="#f8fafc",
                           outline="#0f172a", width=3)
    draw.rectangle((x0, y0, x1, y0 + title_height),
                   fill="#dbeafe", outline="#0f172a", width=2)
    draw_centered_lines(draw, (x0, y0, x1, y0 + title_height),
                        [title], font(23, True))
    draw.line((x0, y0 + title_height, x1, y0 + title_height),
              fill="#0f172a", width=2)
    draw.line((x0, method_start - 8, x1, method_start - 8),
              fill="#0f172a", width=2)

    body_font = font(17)
    y = attr_start
    for item in attributes:
        draw.text((x0 + 14, y), item, font=body_font, fill="#334155")
        y += line_gap
    y = method_start + 5
    for item in methods:
        draw.text((x0 + 14, y), item, font=body_font, fill="#1e3a8a")
        y += line_gap


def draw_diamond(draw: ImageDraw.ImageDraw, center: tuple[int, int],
                 size: int = 11) -> None:
    x, y = center
    points = [(x, y - size), (x + size, y), (x, y + size), (x - size, y)]
    draw.polygon(points, fill="#0f172a", outline="#0f172a")


def draw_triangle(draw: ImageDraw.ImageDraw, point: tuple[int, int],
                  direction: str = "down", size: int = 14) -> None:
    x, y = point
    if direction == "down":
        points = [(x, y), (x - size, y - size), (x + size, y - size)]
    elif direction == "up":
        points = [(x, y), (x - size, y + size), (x + size, y + size)]
    elif direction == "left":
        points = [(x, y), (x + size, y - size), (x + size, y + size)]
    else:
        points = [(x, y), (x - size, y - size), (x - size, y + size)]
    draw.polygon(points, fill="#ffffff", outline="#0f172a", width=2)


def draw_class_diagram() -> None:
    image = Image.new("RGB", (1800, 1220), "white")
    draw = ImageDraw.Draw(image)
    title_font = font(34, True)
    image_title = "图书借阅系统 UML 类图"
    width, _ = text_size(draw, image_title, title_font)
    draw.text(((1800 - width) // 2, 20), image_title,
              font=title_font, fill="#0f172a")

    borrow_rule = (60, 85, 450, 340)
    student = (60, 500, 450, 720)
    teacher = (60, 790, 450, 1010)
    reader = (600, 85, 1000, 365)
    book = (1120, 85, 1540, 365)
    record = (1110, 470, 1590, 790)
    library = (570, 850, 1450, 1160)

    draw_box(draw, borrow_rule, "BorrowRule",
             ["- 抽象借阅规则"],
             ["+ readerType(): string", "+ maxBorrowCount(): size_t",
              "+ borrowDays(): int"])
    draw_box(draw, student, "StudentBorrowRule",
             ["- 学生规则"],
             ["+ maxBorrowCount() = 3", "+ borrowDays() = 30"])
    draw_box(draw, teacher, "TeacherBorrowRule",
             ["- 教师规则"],
             ["+ maxBorrowCount() = 8", "+ borrowDays() = 90"])
    draw_box(draw, reader, "Reader",
             ["- id: string", "- name: string",
              "- rule: unique_ptr<BorrowRule>"],
             ["+ canBorrow(count): bool", "+ toCsv(): string"])
    draw_box(draw, book, "Book",
             ["- isbn/title/author: string", "- totalCopies: int",
              "- availableCopies: int"],
             ["+ borrowCopy(): bool", "+ returnCopy(): bool",
              "+ isAvailable(): bool"])
    draw_box(draw, record, "BorrowRecord",
             ["- recordId: int", "- readerId/isbn: string",
              "- borrowDate/dueDate: string", "- returnDate: string"],
             ["+ markReturned(date): void", "+ isReturned(): bool",
              "+ toCsv(): string"])
    draw_box(draw, library, "Library",
             ["- books: vector<Book>", "- readers: vector<Reader>",
              "- records: vector<BorrowRecord>", "- dataDir: string"],
             ["+ borrowBook(readerId, isbn): bool",
              "+ returnBook(readerId, isbn): bool",
              "+ searchBooks(keyword): vector<Book>", "+ save(): bool"])

    ink = "#0f172a"
    draw.line((255, 500, 255, 340), fill=ink, width=3)
    draw_triangle(draw, (255, 360), "up")
    draw.line((255, 790, 25, 790, 25, 220, 60, 220), fill=ink, width=3)
    draw_triangle(draw, (75, 220), "left")

    draw.line((600, 225, 450, 225), fill=ink, width=3)
    draw_diamond(draw, (585, 225), 12)
    draw.text((510, 190), "1", font=font(18, True), fill="#475569")

    draw.line((780, 850, 800, 365), fill=ink, width=3)
    draw_diamond(draw, (783, 820), 12)
    draw.line((980, 850, 1320, 365), fill=ink, width=3)
    draw_diamond(draw, (1005, 813), 12)
    draw.line((1180, 850, 1350, 790), fill=ink, width=3)
    draw_diamond(draw, (1190, 832), 12)

    draw.line((1320, 470, 1320, 365), fill="#64748b", width=2)
    for y in range(475, 360, -14):
        draw.line((1320, y, 1320, min(y + 7, 365)), fill="#64748b", width=2)
    draw.line((1110, 610, 1000, 610, 1000, 260), fill="#64748b", width=2)
    for y in range(605, 265, -14):
        draw.line((1000, y, 1000, max(y - 7, 260)), fill="#64748b", width=2)
    draw.text((1220, 400), "关联 isbn", font=font(17), fill="#475569")
    draw.text((1015, 430), "关联 readerId", font=font(17), fill="#475569")

    legend_y = 1130
    draw.line((90, legend_y, 150, legend_y), fill=ink, width=3)
    draw_diamond(draw, (105, legend_y), 9)
    draw.text((160, legend_y - 13), "组合", font=font(18), fill=ink)
    draw.line((280, legend_y, 340, legend_y), fill=ink, width=3)
    draw_triangle(draw, (295, legend_y), "left")
    draw.text((350, legend_y - 13), "继承", font=font(18), fill=ink)
    draw.text((470, legend_y - 13), "虚线：关联/依赖",
              font=font(18), fill=ink)

    ASSET_DIR.mkdir(parents=True, exist_ok=True)
    image.save(ASSET_DIR / "class_diagram.png", dpi=(180, 180))


def draw_flow_box(draw: ImageDraw.ImageDraw, box: tuple[int, int, int, int],
                  text: str, fill: str = "#eff6ff") -> None:
    draw.rounded_rectangle(box, radius=18, fill=fill,
                           outline="#0f172a", width=3)
    draw_centered_lines(draw, box, wrap_text(draw, text, font(22), box[2] - box[0] - 30),
                        font(22))


def draw_flow_diamond(draw: ImageDraw.ImageDraw, center: tuple[int, int],
                      text: str, width: int = 260, height: int = 130) -> None:
    x, y = center
    points = [(x, y - height // 2), (x + width // 2, y),
              (x, y + height // 2), (x - width // 2, y)]
    draw.polygon(points, fill="#fef3c7", outline="#0f172a")
    draw.line(points + [points[0]], fill="#0f172a", width=3, joint="curve")
    draw_centered_lines(draw, (x - width // 2 + 25, y - 42,
                               x + width // 2 - 25, y + 42),
                        wrap_text(draw, text, font(22), width - 70), font(22))


def arrow(draw: ImageDraw.ImageDraw, start: tuple[int, int],
          end: tuple[int, int]) -> None:
    draw.line((start, end), fill="#0f172a", width=3)
    dx = end[0] - start[0]
    dy = end[1] - start[1]
    if abs(dy) >= abs(dx):
        draw_triangle(draw, end, "down" if dy > 0 else "up", 12)
    else:
        draw_triangle(draw, end, "right" if dx > 0 else "left", 12)


def draw_borrow_flow() -> None:
    image = Image.new("RGB", (1280, 1320), "white")
    draw = ImageDraw.Draw(image)
    title = "Library::borrowBook() 核心函数流程图"
    title_font = font(34, True)
    width, _ = text_size(draw, title, title_font)
    draw.text(((1280 - width) // 2, 24), title,
              font=title_font, fill="#0f172a")

    center_x = 500
    start = (center_x - 100, 100, center_x + 100, 165)
    input_box = (center_x - 220, 205, center_x + 220, 285)
    find_box = (center_x - 220, 325, center_x + 220, 405)
    exists = (center_x, 495)
    available = (center_x, 675)
    limit = (center_x, 855)
    borrow_box = (center_x - 220, 950, center_x + 220, 1030)
    date_box = (center_x - 220, 1070, center_x + 220, 1150)
    end = (center_x - 100, 1210, center_x + 100, 1275)

    draw.rounded_rectangle(start, radius=32, fill="#dcfce7",
                           outline="#0f172a", width=3)
    draw_centered_lines(draw, start, ["开始"], font(22, True))
    draw_flow_box(draw, input_box, "输入 readerId、isbn")
    draw_flow_box(draw, find_box, "查找图书与读者")
    draw_flow_diamond(draw, exists, "两者都存在？")
    draw_flow_diamond(draw, available, "存在可借副本？")
    draw_flow_diamond(draw, limit, "低于借阅上限？")
    draw_flow_box(draw, borrow_box, "borrowCopy() 减少副本")
    draw_flow_box(draw, date_box, "计算 borrowDate 和 dueDate")
    draw_flow_box(draw, (center_x - 220, 1190, center_x + 220, 1270),
                  "新增 BorrowRecord，返回成功")

    arrow(draw, (center_x, 165), (center_x, 205))
    arrow(draw, (center_x, 285), (center_x, 325))
    arrow(draw, (center_x, 405), (center_x, 430))
    arrow(draw, (center_x, 560), (center_x, 610))
    arrow(draw, (center_x, 740), (center_x, 790))
    arrow(draw, (center_x, 920), (center_x, 950))
    arrow(draw, (center_x, 1030), (center_x, 1070))
    arrow(draw, (center_x, 1150), (center_x, 1190))

    error_font = font(20)
    error_y = [495, 675, 855]
    labels = ["读者或图书不存在", "无可用副本", "达到借阅上限"]
    for y, label in zip(error_y, labels):
        box = (center_x + 190, y - 40, center_x + 445, y + 40)
        draw_flow_box(draw, box, label, "#fee2e2")
        arrow(draw, (center_x + 130, y), (center_x + 190, y))
        draw.line((center_x + 445, y, center_x + 505, y), fill="#0f172a", width=3)
        draw.line((center_x + 505, y, center_x + 505, 1305), fill="#0f172a", width=3)
        draw.line((center_x + 220, 1305, center_x + 505, 1305), fill="#0f172a", width=3)
    draw.text((center_x + 70, 585), "是", font=error_font, fill="#166534")
    draw.text((center_x + 70, 765), "是", font=error_font, fill="#166534")
    draw.text((center_x + 70, 945), "是", font=error_font, fill="#166534")
    draw.text((center_x + 170, 430), "否", font=error_font, fill="#b91c1c")
    draw.text((center_x + 170, 610), "否", font=error_font, fill="#b91c1c")
    draw.text((center_x + 170, 790), "否", font=error_font, fill="#b91c1c")

    ASSET_DIR.mkdir(parents=True, exist_ok=True)
    image.save(ASSET_DIR / "borrow_flow.png", dpi=(180, 180))


def draw_terminal_text(source: Path, output: Path, title: str) -> None:
    width = 1500
    image_preview = Image.new("RGB", (width, 100), "white")
    preview_draw = ImageDraw.Draw(image_preview)
    preview_text = source.read_text(encoding="utf-8") if source.exists() else "No output."
    preview_lines: list[str] = []
    for line in preview_text.splitlines():
        preview_lines.extend(wrap_text(preview_draw, line, font(19), width - 60) or [""])

    displayed_lines = preview_lines[:55]
    height = max(940, 115 + len(displayed_lines) * 24)
    image = Image.new("RGB", (width, height), "#0b1220")
    draw = ImageDraw.Draw(image)
    draw.rectangle((0, 0, width, 54), fill="#1f2937")
    for x, color in [(24, "#ef4444"), (52, "#f59e0b"), (80, "#22c55e")]:
        draw.ellipse((x, 17, x + 18, 35), fill=color)
    draw.text((120, 12), title, font=font(24, True), fill="#e5e7eb")

    y = 78
    for line in displayed_lines:
        draw.text((28, y), line, font=font(19), fill="#d1d5db")
        y += 24

    from datetime import datetime
    stamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    draw.text((width - 320, height - 36), stamp,
              font=font(18), fill="#94a3b8")
    image.save(output, dpi=(150, 150))


def draw_text_assets() -> None:
    draw_terminal_text(ASSET_DIR / "console_run.txt",
                       ASSET_DIR / "console_run.png",
                       "PowerShell - library_system.exe")
    draw_terminal_text(ASSET_DIR / "debug_runtime.txt",
                       ASSET_DIR / "debug_runtime.png",
                       "GDB debug record - invalid recordId")
    draw_terminal_text(ASSET_DIR / "submission_record.txt",
                       ASSET_DIR / "submission_record.png",
                       "Build, test and file inventory")


def main() -> None:
    draw_class_diagram()
    draw_borrow_flow()
    draw_text_assets()


if __name__ == "__main__":
    main()
