$:.unshift File.join(File.dirname(__FILE__), '..' , 'clutter')
$:.unshift File.join(File.dirname(__FILE__), '..' , 'clutter-ply')
$:.unshift File.join(File.dirname(__FILE__))
require 'clutter-init'
require 'clutter_ply'
require 'test/unit'

class TC_ClutterPlyModel < Test::Unit::TestCase
  CUBE_PLY = File.join(File.dirname(__FILE__), 'cube.ply')

  def setup
    @model = Clutter::Ply::Model.new(CUBE_PLY)
  end

  def teardown
    @model = nil
  end

  def test_new_from_file
    assert_kind_of(Clutter::Ply::Model.new(CUBE_PLY),
                   Clutter::Ply::Model)
  end

  def test_new_from_file_fail
    assert_raise(Clutter::Ply::Data::Error) do
      Clutter::Ply::Model.new("/not/a/real/file/hopefully")
    end
  end

  def test_material
    material = Cogl::Material.new
    # We can't compare materials to decide if it's the same object
    assert_kind_of(@model.material, Cogl::Material)
    assert_kind_of(@model.material = material, Cogl::Material)
    assert_equal(@model.set_material(material), @model)
  end

  def test_data
    assert_kind_of(@model.data, Clutter::Ply::Data)
    data = Clutter::Ply::Data.new
    assert_equal(@model.data = data, data)
    assert_equal(@model.set_data(data), @model)
    assert_equal(@model.data, data)
  end

  def test_fit_to_allocation
    assert_equal(@model.fit_to_allocation = true, true)
    assert_equal(@model.set_fit_to_allocation(true), @model)
    assert_equal(@model.fit_to_allocation?, true)
  end
end
