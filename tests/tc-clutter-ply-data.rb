$:.unshift File.join(File.dirname(__FILE__), '..' , 'clutter')
$:.unshift File.join(File.dirname(__FILE__), '..' , 'clutter-ply')
$:.unshift File.join(File.dirname(__FILE__))
require 'clutter-init'
require 'clutter_ply'
require 'test/unit'

class TC_ClutterPlyData < Test::Unit::TestCase
  CUBE_PLY = File.join(File.dirname(__FILE__), 'cube.ply')

  def setup
    @data = Clutter::Ply::Data.new
  end

  def teardown
    @data = nil
  end

  def test_load
    assert_equal(@data.load(CUBE_PLY), @data)
  end

  def test_load_fail
    assert_raise(Clutter::Ply::Data::Error) do
      @data.load("/not/a/real/file/hopefully")
    end
  end

  def test_render
    assert_equal(@data.render, @data)
  end

  def check_vertex_equal(va, vb)
    assert_in_delta(va.x, vb.x, 1e-4)
    assert_in_delta(va.y, vb.y, 1e-4)
    assert_in_delta(va.z, vb.z, 1e-4)
  end

  def test_extents
    @data.load(CUBE_PLY)
    assert_kind_of(Array, @data.extents)
    assert_equal(@data.extents.length, 2)
    check_vertex_equal(@data.extents[0],
                       Clutter::Vertex.new(-1, -1, -1))
    check_vertex_equal(@data.extents[1],
                       Clutter::Vertex.new(1, 1, 1))
  end
end
