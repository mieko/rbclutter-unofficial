require 'test/unit'
$:.unshift File.join(File.dirname(__FILE__), '..' , 'clutter')
$:.unshift File.join(File.dirname(__FILE__))
require 'clutter-init'

class TC_CoglTextureMethods < Test::Unit::TestCase
  TEX_WIDTH = 32
  TEX_HEIGHT = 16

  def setup
    # Create a small red texture from a string
    @tex = Cogl::Texture.new(TEX_WIDTH,
                             TEX_HEIGHT,
                             nil,
                             Cogl::PixelFormat::RGBA_8888,
                             Cogl::PixelFormat::RGBA_8888_PRE,
                             nil,
                             "\xff\x00\x00\xff" * TEX_WIDTH * TEX_HEIGHT);
  end

  def teardown
    @tex = nil
  end

  def test_width
    assert_equal(@tex.width, TEX_WIDTH)
  end

  def test_height
    assert_equal(@tex.height, TEX_HEIGHT)
  end

  def test_format
    assert_equal(@tex.format, Cogl::PixelFormat::RGBA_8888_PRE)
    assert_kind_of(@tex.format, Cogl::PixelFormat)
  end

  def test_rowstride
    assert_equal(@tex.rowstride, TEX_WIDTH * 4)
  end

  def test_max_waste
    assert_equal(@tex.max_waste, -1)
  end

  def test_sliced
    assert_equal(@tex.sliced?, false)
  end

  def test_gl_texture
    gl_texture = @tex.gl_texture
    assert_kind_of(gl_texture, Array)
    assert_equal(gl_texture.length, 2)
    gl_texture.each { |x| assert_kind_of(x, Fixnum) }
  end

  def check_tex_data(data)
    length = if data.respond_to?(:bytesize)
               data.bytesize # only in Ruby >= 1.9
             else
               data.length # this is the same as the byte length in <= 1.8
             end
    assert_equal(length, TEX_WIDTH * TEX_HEIGHT * 4)
    i = 0
    data.each_byte do |byte|
      case (i & 3)
      when 0
      when 3
        assert_equal(byte, 0xff)
      else
        assert_equal(byte, 0x00)
      end

      i += 1
    end
  end

  def test_get_data
    check_tex_data(@tex.get_data)
    check_tex_data(@tex.get_data(Cogl::PixelFormat::RGBA_8888, TEX_WIDTH * 4))
  end

  def check_pixel(x, y, pixel)
    data = @tex.get_data
    pos = y * TEX_WIDTH * 4 + x * 4
    assert_equal(data[pos...(pos + 4)], pixel)
  end

  def test_set_region
    @tex.set_region(0, 0, 3, 4, 1, 1, 1, 1,
                    Cogl::PixelFormat::RGBA_8888_PRE, 4,
                    "\x00\xff\xff\xff")
    check_pixel(2, 4, "\xff\x00\x00\xff")
    check_pixel(3, 4, "\x00\xff\xff\xff")
    check_pixel(4, 4, "\xff\x00\x00\xff")
  end
end

class TC_CoglTextureConstructors < Test::Unit::TestCase
  RED_TEX = File.join(File.dirname(__FILE__), "redtex.png")
  RED_TEX_WIDTH = 32
  RED_TEX_HEIGHT = 64

  def check_pixel(tex, x, y, pixel)
    data = tex.get_data
    pos = x * pixel.length + y * tex.rowstride
    assert_equal(data[pos...(pos + pixel.length)], pixel)
  end

  def test_file
    tex = Cogl::Texture.new(RED_TEX)
    assert_equal(tex.width, RED_TEX_WIDTH)
    assert_equal(tex.height, RED_TEX_HEIGHT)
    check_pixel(tex, 0, 0, "\xff\x00\x00")
    check_pixel(tex, 0, RED_TEX_HEIGHT / 2, "\x00\x00\xff")
  end

  def test_file_convert
    tex = Cogl::Texture.new(RED_TEX, Cogl::Texture::NONE,
                            Cogl::PixelFormat::RGBA_8888_PRE)
    assert_equal(tex.width, RED_TEX_WIDTH)
    assert_equal(tex.height, RED_TEX_HEIGHT)
    check_pixel(tex, 0, 0, "\xff\x00\x00\xff")
    check_pixel(tex, 0, RED_TEX_HEIGHT / 2, "\x00\x00\xff\xff")
  end

  def test_file_fail
    assert_raise(GLib::FileError) do
      tex = Cogl::Texture.new("/not/a/real/file/hopefully")
    end
  end

  def test_data
    tex = Cogl::Texture.new(1, 2, Cogl::Texture::NONE,
                            Cogl::PixelFormat::RGB_888,
                            Cogl::PixelFormat::RGBA_8888_PRE,
                            8,
                            "\xff\x00\x00" + "\x80" * 5 +
                            "\x00\xff\x00" + "\x80" * 5)
    check_pixel(tex, 0, 0, "\xff\x00\x00\xff")
    check_pixel(tex, 0, 1, "\x00\xff\x00\xff")
  end

  def test_data_fail
    assert_raise(ArgumentError) do
      Cogl::Texture.new(100, 100, nil,
                        Cogl::PixelFormat::RGB_888,
                        nil,
                        100,
                        "shortstring")
    end
  end

  def test_foreign
    red_tex = Cogl::Texture.new(RED_TEX,
                                Cogl::Texture::NO_ATLAS)
    foreign_tex = Cogl::Texture.new(*red_tex.gl_texture,
                                    RED_TEX_WIDTH,
                                    RED_TEX_HEIGHT,
                                    0, 0,
                                    Cogl::PixelFormat::RGB_888)
    assert_equal(foreign_tex.width, RED_TEX_WIDTH)
    assert_equal(foreign_tex.height, RED_TEX_HEIGHT)
    assert_equal(foreign_tex.format, Cogl::PixelFormat::RGB_888)
    check_pixel(foreign_tex, 0, 0, "\xff\x00\x00")
    check_pixel(foreign_tex, 0, RED_TEX_HEIGHT / 2, "\x00\x00\xff")
  end

  def test_bitmap
    bmp = Cogl::Bitmap.new(RED_TEX)
    tex = Cogl::Texture.new(bmp, Cogl::Texture::NONE,
                            Cogl::PixelFormat::RGBA_8888_PRE)
    assert_equal(tex.width, RED_TEX_WIDTH)
    assert_equal(tex.height, RED_TEX_HEIGHT)
    assert_equal(tex.format, Cogl::PixelFormat::RGBA_8888_PRE)
    check_pixel(tex, 0, 0, "\xff\x00\x00\xff")
    check_pixel(tex, 0, RED_TEX_HEIGHT / 2, "\x00\x00\xff\xff")
  end

  def test_sub_texture
    tex = Cogl::Texture.new(RED_TEX)
    sub_tex = Cogl::Texture.new(tex, 0, RED_TEX_HEIGHT / 2,
                                RED_TEX_WIDTH, RED_TEX_HEIGHT / 2)
    assert_equal(sub_tex.width, RED_TEX_WIDTH)
    assert_equal(sub_tex.height, RED_TEX_HEIGHT / 2)
    assert_equal(sub_tex.format, Cogl::PixelFormat::RGB_888)
    check_pixel(sub_tex, 0, 0, "\x00\x00\xff")
    check_pixel(sub_tex, 0, RED_TEX_HEIGHT / 4, "\x00\x00\xff\x00")
  end
end
