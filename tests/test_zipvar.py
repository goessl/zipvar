def test_czip_var():
    from zipvar._zipvar import zip_var
    
    assert tuple(zip_var()) == ()
    assert tuple(zip_var([])) == ()
    assert tuple(zip_var([], [])) == ()
    
    assert tuple(zip_var([1])) == ((1,),)
    assert tuple(zip_var([1, 2])) == ((1,), (2,))
    assert tuple(zip_var([1, 2], [])) == ((1,), (2,))
    assert tuple(zip_var([1, 2], [3])) == ((1, 3), (2,))
    
    assert tuple(zip_var((1, 2, 3), [4, 5, 6, 7, 8], set())) \
            == ((1, 4), (2, 5), (3, 6), (7,), (8,))

def test_pyzip_var():
    from zipvar.pyzipvar import zip_var
    
    assert tuple(zip_var()) == ()
    assert tuple(zip_var([])) == ()
    assert tuple(zip_var([], [])) == ()
    
    assert tuple(zip_var([1])) == ((1,),)
    assert tuple(zip_var([1, 2])) == ((1,), (2,))
    assert tuple(zip_var([1, 2], [])) == ((1,), (2,))
    assert tuple(zip_var([1, 2], [3])) == ((1, 3), (2,))
    
    assert tuple(zip_var((1, 2, 3), [4, 5, 6, 7, 8], set())) \
            == ((1, 4), (2, 5), (3, 6), (7,), (8,))


if __name__ == '__main__':
    test_czip_var()
    test_pyzip_var()
    print('All tests passed!')
