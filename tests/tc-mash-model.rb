$:.unshift File.join(File.dirname(__FILE__), '..' , 'clutter')
$:.unshift File.join(File.dirname(__FILE__), '..' , 'mash')
$:.unshift File.join(File.dirname(__FILE__))
require 'clutter-init'
require 'mash'
require 'test/unit'

class TC_MashModel < Test::Unit::TestCase
  CUBE_PLY = File.join(File.dirname(__FILE__), 'cube.ply')

  def setup
    @model = Mash::Model.new(CUBE_PLY)
  end

  def teardown
    @model = nil
  end

  def test_new_from_file
    assert_kind_of(Mash::Model,
                   Mash::Model.new(CUBE_PLY))
  end

  def test_new_from_file_fail
    assert_raise(Mash::Data::Error) do
      Mash::Model.new("/not/a/real/file/hopefully")
    end
  end

  def test_material
    material = Cogl::Material.new
    # We can't compare materials to decide if it's the same object
    assert_kind_of(Cogl::Material, @model.material)
    assert_kind_of(Cogl::Material, @model.material = material)
    assert_equal(@model.set_material(material), @model)
  end

  def test_data
    assert_kind_of(Mash::Data, @model.data)
    data = Mash::Data.new
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
