$:.unshift File.join(File.dirname(__FILE__), '..' , 'clutter')
$:.unshift File.join(File.dirname(__FILE__))
require 'clutter-init'
require 'test/unit'

class TC_CoglVertexBuffer < Test::Unit::TestCase
  def setup
    # Create an empty vertex buffer
    @vertex_buffer = Cogl::VertexBuffer.new(4)
  end

  def teardown
    @vertex_buffer = nil
  end

  def test_get_n_vertices
    assert_equal(@vertex_buffer.n_vertices, 4)
  end

  def test_add_string
    assert_equal(@vertex_buffer.add("gl_Vertex", 2,
                                    Cogl::AttributeType::UNSIGNED_BYTE,
                                    false,
                                    0,
                                    "12345678"), @vertex_buffer)
    @vertex_buffer.submit
  end

  def test_add_array
    assert_equal(@vertex_buffer.add("gl_Vertex", 2,
                                    Cogl::AttributeType::UNSIGNED_BYTE,
                                    false,
                                    0,
                                    [ 1, 2, 3, 4, 5, 6, 7, 8 ]), @vertex_buffer)
    @vertex_buffer.submit
  end

  def test_add_string_short
    assert_equal(@vertex_buffer.add("gl_Vertex", 2,
                                    Cogl::AttributeType::UNSIGNED_BYTE,
                                    false,
                                    0,
                                    "1234567"), @vertex_buffer)
    assert_raise(ArgumentError) do
      @vertex_buffer.submit
    end
  end

  def test_add_array_short
    assert_equal(@vertex_buffer.add("gl_Vertex", 2,
                                    Cogl::AttributeType::UNSIGNED_BYTE,
                                    false,
                                    0,
                                    [ 1, 2, 3, 4, 5, 6, 7 ]), @vertex_buffer)
    assert_raise(ArgumentError) do
      @vertex_buffer.submit
    end
  end

  def add_test_attribute
    @vertex_buffer.add("gl_Vertex", 2,
                       Cogl::AttributeType::SHORT,
                       false,
                       0,
                       "1234567812345678")
    @vertex_buffer.submit
  end

  def test_enable
    add_test_attribute
    assert_equal(@vertex_buffer.enable("gl_Vertex"), @vertex_buffer)
  end

  def test_disable
    add_test_attribute
    assert_equal(@vertex_buffer.disable("gl_Vertex"), @vertex_buffer)
  end

  def test_delete
    add_test_attribute
    assert_equal(@vertex_buffer.delete("gl_Vertex"), @vertex_buffer)
  end

  def test_draw_short
    add_test_attribute
    assert_equal(@vertex_buffer.draw(Cogl::VerticesMode::TRIANGLE_STRIP),
                 @vertex_buffer)
  end

  def test_draw_full
    add_test_attribute
    assert_equal(@vertex_buffer.draw(Cogl::VerticesMode::TRIANGLE_STRIP, 0, 4),
                 @vertex_buffer)
  end

  def test_draw_bad_first
    add_test_attribute
    assert_raise(ArgumentError) do
      @vertex_buffer.draw(Cogl::VerticesMode::TRIANGLE_STRIP, 4)
    end
  end

  def test_draw_bad_offset
    add_test_attribute
    assert_raise(ArgumentError) do
      @vertex_buffer.draw(Cogl::VerticesMode::TRIANGLE_STRIP, 0, 5)
    end
  end

  def test_indices_new_string
    indices = Cogl::VertexBuffer::Indices.new(Cogl::IndicesType::BYTE, "a")
    assert_kind_of(Cogl::VertexBuffer::Indices, indices)
  end

  def test_indices_new_array
    indices = Cogl::VertexBuffer::Indices.new(Cogl::IndicesType::BYTE, [ 1 ])
    assert_kind_of(Cogl::VertexBuffer::Indices, indices)
  end

  def test_get_for_quads
    indices = Cogl::VertexBuffer::Indices.get_for_quads(4)
    assert_kind_of(Cogl::VertexBuffer::Indices, indices)
  end

  def test_draw_elements
    indices = Cogl::VertexBuffer::Indices.get_for_quads(4)
    assert_equal(@vertex_buffer.draw_elements(Cogl::VerticesMode::TRIANGLE_FAN,
                                              indices,
                                              0, 3,
                                              0, 4), @vertex_buffer)
  end
end
