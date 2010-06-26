$:.unshift File.join(File.dirname(__FILE__), '..' , 'clutter')
$:.unshift File.join(File.dirname(__FILE__))
require 'clutter-init'
require 'test/unit'
require 'pango'

class TC_ClutterTextConstructors < Test::Unit::TestCase
  def test_initialize_empty
    text = Clutter::Text.new
    assert_equal(text.text, "")
  end

  def test_initialize_with_font
    text = Clutter::Text.new("Sans 30px")
    assert_equal(text.font_name, "Sans 30px")
    assert_equal(text.text, "")
  end

  def test_initialize_with_text
    text = Clutter::Text.new("Sans 30px", "123456789")
    assert_equal(text.font_name, "Sans 30px")
    assert_equal(text.text, "123456789")
  end

  def test_initialize_with_full
    color = Clutter::Color.new(255, 0, 0, 255)
    text = Clutter::Text.new("Sans 30px", "123456789", color)
    assert_equal(text.font_name, "Sans 30px")
    assert_equal(text.text, "123456789")
    assert_equal(text.color, color)
  end
end

class TC_ClutterText < Test::Unit::TestCase
  def setup
    @text = Clutter::Text.new(nil, "123456789")
  end

  def teardown
    @text = nil
  end

  def test_text
    assert_equal(@text.text, "123456789")
    assert_equal(@text.set_text("pie"), @text)
    assert_equal(@text.text = "pie", "pie")
    assert_equal(@text.text, "pie")
  end

  def test_markup
    assert_equal(@text.set_markup("<b>pie</b>"), @text)
    assert_equal(@text.markup = "<b>pie</b>", "<b>pie</b>")
    assert_equal(@text.text, "pie")
    assert_equal(@text.use_markup?, true)
  end

  def test_color
    col = Clutter::Color.new(255, 0, 0, 255)
    assert_equal(@text.set_color(col), @text)
    assert_equal(@text.color = col, col)
    assert_equal(@text.color, col)
  end

  def test_font_name
    assert_equal(@text.font_name = "Serif 10px", "Serif 10px")
    assert_equal(@text.set_font_name("Serif 10px"), @text)
    assert_equal(@text.font_name, "Serif 10px")
  end

  def test_font_description
    pfd = Pango::FontDescription.new("Serif 15px")
    assert_equal(@text.font_description = pfd, pfd)
    assert_equal(@text.set_font_description(pfd), @text)
    assert_equal(@text.font_description, pfd)
  end

  def test_ellipsize
    assert_equal(@text.ellipsize = Pango::ELLIPSIZE_MIDDLE,
                 Pango::ELLIPSIZE_MIDDLE)
    assert_equal(@text.set_ellipsize(Pango::ELLIPSIZE_MIDDLE), @text)
    assert_equal(@text.ellipsize, Pango::ELLIPSIZE_MIDDLE)
  end

  def test_line_wrap
    assert_equal(@text.line_wrap = false, false)
    assert_equal(@text.set_line_wrap(false), @text)
    assert_equal(@text.line_wrap?, false)
  end

  def test_line_wrap_mode
    assert_equal(@text.line_wrap_mode = Pango::WRAP_CHAR, Pango::WRAP_CHAR)
    assert_equal(@text.set_line_wrap_mode(Pango::WRAP_CHAR), @text)
    assert_equal(@text.line_wrap_mode, Pango::WRAP_CHAR)
  end

  def test_layout
    assert_kind_of(Pango::Layout, @text.layout)
    assert_equal(@text.layout.text, @text.text)
  end

  def attributes_equal(attr1, attr2)
    it1 = attr1.iterator
    it2 = attr2.iterator
    while true
      return false if it1.get != it2.get
      has_next = it1.next!
      return false if has_next != it2.next!
      break unless has_next
    end
    true
  end

  def test_attributes
    attr = Pango::AttrList.new
    attr.insert(Pango::AttrWeight.new(Pango::WEIGHT_BOOK))
    assert_equal(@text.attributes = attr, attr)
    assert_equal(@text.set_attributes(attr), @text)
    assert(attributes_equal(@text.attributes, attr),
           "Attributes do not match")
  end

  def test_use_markup
    assert_equal(@text.use_markup = true, true)
    assert_equal(@text.set_use_markup(true), @text)
    assert_equal(@text.use_markup?, true)
  end

  def test_line_alignment
    assert_equal(@text.line_alignment = Pango::ALIGN_CENTER,
                 Pango::ALIGN_CENTER)
    assert_equal(@text.set_line_alignment(Pango::ALIGN_CENTER), @text)
    assert_equal(@text.line_alignment, Pango::ALIGN_CENTER)
  end

  def test_justify
    assert_equal(@text.justify = true, true)
    assert_equal(@text.set_justify(true), @text)
    assert_equal(@text.justify?, true)
  end

  def test_insert_unichar
    assert_equal(@text.insert_unichar(?\(), @text)
    # Under Ruby 1.8, insert_unichar doesn't accept 1-character strings
    if ")".respond_to?(:ord)
      assert_equal(@text.insert_unichar(")"), @text)
    else
      assert_equal(@text.insert_unichar(?\)), @text)
    end
    assert_equal(@text.text, "123456789()")
  end

  def test_delete_chars
    assert_equal(@text.delete_chars(3), @text)
    assert_equal(@text.text, "123456")
  end

  def test_insert_text
    assert_equal(@text.insert_text("()"), @text)
    assert_equal(@text.insert_text("[]", 0), @text)
    assert_equal(@text.insert_text("--", 3), @text)
    assert_equal(@text << ".", @text)
    assert_equal(@text.text, "[]1--23456789().")
  end

  def test_delete_text
    assert_equal(@text.delete_text(2, 3), @text)
    assert_equal(@text.text, "12456789")
  end

  def test_get_chars
    assert_equal(@text.get_chars(7), "89")
    assert_equal(@text.get_chars(7, 8), "8")
    assert_equal(@text.get_chars(7, -1), "89")
    assert_equal(@text.get_chars(0..2), "123")
    assert_equal(@text.get_chars(0...2), "12")
  end

  def test_editable
    assert_equal(@text.editable = true, true)
    assert_equal(@text.set_editable(true), @text)
    assert_equal(@text.editable?, true)
  end

  def test_activatable
    assert_equal(@text.activatable = true, true)
    assert_equal(@text.set_activatable(true), @text)
    assert_equal(@text.activatable?, true)
  end

  def test_cursor_position
    assert_equal(@text.position = 2, 2)
    assert_equal(@text.set_position(2), @text)
    assert_equal(@text.position, 2)
  end

  def test_cursor_visible
    assert_equal(@text.cursor_visible = true, true)
    assert_equal(@text.set_cursor_visible(true), @text)
    assert_equal(@text.cursor_visible?, true)
  end

  def test_cursor_color
    col = Clutter::Color.new(255, 255, 0, 255)
    assert_equal(@text.cursor_color = col, col)
    assert_equal(@text.set_cursor_color(col), @text)
    assert_equal(@text.cursor_color, col)
  end

  def test_cursor_size
    assert_equal(@text.cursor_size = 70, 70)
    assert_equal(@text.set_cursor_size(70), @text)
    assert_equal(@text.cursor_size, 70)
  end

  def test_selectable
    assert_equal(@text.selectable = true, true)
    assert_equal(@text.set_selectable(true), @text)
    assert_equal(@text.selectable?, true)
  end

  def test_selection_bound
    @text.position = 1
    assert_equal(@text.selection_bound = 3, 3)
    assert_equal(@text.set_selection_bound(3), @text)
    assert_equal(@text.selection_bound, 3)
    assert_equal(@text.selection, "23")
  end

  def test_set_selection
    assert_equal(@text.set_selection(1, 3), @text)
    assert_equal(@text.selection, "23")
  end

  def test_selection_color
    col = Clutter::Color.new(255, 255, 0, 255)
    assert_equal(@text.selection_color = col, col)
    assert_equal(@text.set_selection_color(col), @text)
    assert_equal(@text.selection_color, col)
  end

  def test_delete_selection
    assert_equal(@text.set_selection(1, 3), @text)
    assert_equal(@text.delete_selection, @text)
    assert_equal(@text.text, "1456789")
  end

  def test_password_char
    assert_equal(@text.password_char = 65, 65)
    assert_equal(@text.set_password_char(65), @text)
    assert_equal(@text.password_char, 65)
  end

  def test_max_length
    assert_equal(@text.max_length = 65, 65)
    assert_equal(@text.set_max_length(65), @text)
    assert_equal(@text.max_length, 65)
  end

  def test_single_line_mode
    assert_equal(@text.single_line_mode = true, true)
    assert_equal(@text.set_single_line_mode(true), @text)
    assert_equal(@text.single_line_mode?, true)
  end

  def test_activate
    assert_equal(@text.activate, @text)
  end

  def test_position_to_coords
    assert_equal(@text.position_to_coords(1000), nil)
    assert_kind_of(Array, @text.position_to_coords(0))
    assert_equal(@text.position_to_coords(0).length, 3)
  end

  def test_set_preedit_string
    assert_equal(@text.set_preedit_string("12", nil, 0), @text)
    assert_equal(@text.set_preedit_string("12", Pango::AttrList.new, 0), @text)
  end
end
