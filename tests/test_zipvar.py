from zipvar import zip_var

def test_zip_var():
    assert tuple(zip_var()) == ()
    assert tuple(zip_var([])) == ()
    assert tuple(zip_var([], [])) == ()
    
    assert tuple(zip_var([1])) == ((1,),)
    assert tuple(zip_var([1, 2])) == ((1,), (2,))
    assert tuple(zip_var([1, 2], [])) == ((1,), (2,))
    assert tuple(zip_var([1, 2], [3])) == ((1, 3), (2,))
    
    assert tuple(zip_var((1, 2, 3), [4, 5, 6, 7, 8], set())) \
            == ((1, 4), (2, 5), (3, 6), (7,), (8,))
