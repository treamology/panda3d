// Filename: bitArray.cxx
// Created by:  drose (20Jan06)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////

#include "bitArray.h"

TypeHandle BitArray::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: BitArray::is_zero
//       Access: Published
//  Description: Returns true if the entire bitmask is zero, false
//               otherwise.
////////////////////////////////////////////////////////////////////
bool BitArray::
is_zero() const {
  if (_highest_bits) {
    // If all the infinite highest bits are set, certainly the bitmask
    // is nonzero.
    return false;
  }

  // Start from the high end, since that's more likely to be nonzero.
  Array::const_reverse_iterator ai;
  for (ai = _array.rbegin(); ai != _array.rend(); ++ai) {
    if ((*ai) != 0) {
      return false;
    }
  }
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::set_range
//       Access: Published
//  Description: Sets the indicated range of bits on.
////////////////////////////////////////////////////////////////////
void BitArray::
set_range(int low_bit, int size) {
  int w = low_bit / num_bits_per_word;
  int b = low_bit % num_bits_per_word;

  if (w >= get_num_words() && _highest_bits) {
    // All the highest bits are already on.
    return;
  }
  if (b + size <= num_bits_per_word) {
    // The whole thing fits within one word of the array.
    ensure_has_word(w);
    _array[w].set_range(b, size);
    normalize();
    return;
  }

  while (size > 0) {
    if (size <= num_bits_per_word) {
      // The remainder fits within one word of the array.
      ensure_has_word(w);
      _array[w].set_range(0, size);
      normalize();
      return;
    }

    // Keep going.
    ensure_has_word(w);
    _array[w] = MaskType::all_on();
    size -= num_bits_per_word;
    ++w;

    if (w >= get_num_words() && _highest_bits) {
      // All the highest bits are already on.
      normalize();
      return;
    }
  }
  normalize();
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::clear_range
//       Access: Published
//  Description: Sets the indicated range of bits off.
////////////////////////////////////////////////////////////////////
void BitArray::
clear_range(int low_bit, int size) {
  int w = low_bit / num_bits_per_word;
  int b = low_bit % num_bits_per_word;

  if (w >= get_num_words() && !_highest_bits) {
    // All the highest bits are already off.
    return;
  }
  if (b + size <= num_bits_per_word) {
    // The whole thing fits within one word of the array.
    ensure_has_word(w);
    _array[w].clear_range(b, size);
    normalize();
    return;
  }

  while (size > 0) {
    if (size <= num_bits_per_word) {
      // The remainder fits within one word of the array.
      ensure_has_word(w);
      _array[w].clear_range(0, size);
      normalize();
      return;
    }

    // Keep going.
    ensure_has_word(w);
    _array[w] = MaskType::all_on();
    size -= num_bits_per_word;
    ++w;

    if (w >= get_num_words() && !_highest_bits) {
      // All the highest bits are already off.
      normalize();
      return;
    }
  }
  normalize();
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::invert_in_place
//       Access: Published
//  Description: Inverts all the bits in the BitArray.  This is
//               equivalent to array = ~array.
////////////////////////////////////////////////////////////////////
void BitArray::
invert_in_place() {
  _highest_bits = !_highest_bits;
  Array::iterator ai;
  for (ai = _array.begin(); ai != _array.end(); ++ai) {
    (*ai) = ~(*ai);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::has_bits_in_common
//       Access: Published
//  Description: Returns true if this BitArray has any "one" bits in
//               common with the other one, false otherwise.
//
//               This is equivalent to (array & other) != 0, but may
//               be faster.
////////////////////////////////////////////////////////////////////
bool BitArray::
has_bits_in_common(const BitArray &other) const {
  if (_highest_bits && other._highest_bits) {
    // Yup, in fact we have an infinite number of bits in common.
    return true;
  }

  size_t num_common_words = min(_array.size(), other._array.size());

  // Consider the words that are on top of either array.
  if (other._array.size() < _array.size() && other._highest_bits) {
    // The other array has fewer actual words, and the top n words of
    // the other array are all ones.  We have bits in common if any of
    // our top n words are nonzero.
    Array::const_iterator ai;
    for (ai = _array.begin() + other._array.size(); 
         ai != _array.end(); 
         ++ai) {
      if (!(*ai).is_zero()) {
	return true;
      }
    }
    
  } else if (_array.size() < other._array.size() && _highest_bits) {
    // This array has fewer actual words, and the top n words of this
    // array are all ones.  We have bits in common if any of the the
    // other's top n words are nonzero.
    Array::const_iterator ai;
    for (ai = other._array.begin() + _array.size(); 
         ai != other._array.end(); 
         ++ai) {
      if (!(*ai).is_zero()) {
	return true;
      }
    }
  }

  // Consider the words that both arrays have in common.
  for (size_t i = 0; i < num_common_words; ++i) {
    if (!(_array[i] & other._array[i]).is_zero()) {
      return true;
    }
  }

  // Nope, nothing.
  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::output
//       Access: Published
//  Description: Writes the BitArray out as a hex number.  For a
//               BitArray, this is always the same as output_hex();
//               it's too confusing for the output format to change
//               back and forth at runtime.
////////////////////////////////////////////////////////////////////
void BitArray::
output(ostream &out) const {
  output_hex(out);
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::output_binary
//       Access: Published
//  Description: Writes the BitArray out as a binary number, with
//               spaces every four bits.
////////////////////////////////////////////////////////////////////
void BitArray::
output_binary(ostream &out, int spaces_every) const {
  if (_highest_bits) {
    out << "...1 ";
  }
  int num_bits = max(get_num_bits(), spaces_every);
  for (int i = num_bits - 1; i >= 0; i--) {
    if (spaces_every != 0 && ((i % spaces_every) == spaces_every - 1)) {
      out << ' ';
    }
    out << (get_bit(i) ? '1' : '0');
  }
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::output_hex
//       Access: Published
//  Description: Writes the BitArray out as a hexadecimal number, with
//               spaces every four digits.
////////////////////////////////////////////////////////////////////
void BitArray::
output_hex(ostream &out, int spaces_every) const {
  int num_bits = get_num_bits();
  int num_digits = max((num_bits + 3) / 4, spaces_every);

  if (_highest_bits) {
    out << "...f ";
  }

  for (int i = num_digits - 1; i >= 0; i--) {
    WordType digit = extract(i * 4, 4);
    if (spaces_every != 0 && ((i % spaces_every) == spaces_every - 1)) {
      out << ' ';
    }
    if (digit > 9) {
      out << (char)(digit - 10 + 'a');
    } else {
      out << (char)(digit + '0');
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::write
//       Access: Published
//  Description: Writes the BitArray out as a binary or a hex number,
//               according to the number of bits.
////////////////////////////////////////////////////////////////////
void BitArray::
write(ostream &out, int indent_level) const {
  indent(out, indent_level) << *this << "\n";
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::compare_to
//       Access: Published
//  Description: Returns a number less than zero if this BitArray sorts
//               before the indicated other BitArray, greater than zero
//               if it sorts after, or 0 if they are equivalent.  This
//               is based on the same ordering defined by operator <.
////////////////////////////////////////////////////////////////////
int BitArray::
compare_to(const BitArray &other) const {
  if (_highest_bits != other._highest_bits) {
    return _highest_bits ? 1 : -1;
  }

  int num_words = max(get_num_words(), other.get_num_words());

  // Compare from highest-order to lowest-order word.
  for (int i = num_words - 1; i >= 0; --i) {
    int compare = get_word(i).compare_to(other.get_word(i));
    if (compare != 0) {
      return compare;
    }
  }

  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::operator &=
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BitArray::
operator &= (const BitArray &other) {
  size_t num_common_words = min(_array.size(), other._array.size());

  // Consider the words that are on top of either array.
  if (other._array.size() < _array.size() && !other._highest_bits) {
    // The other array has fewer actual words, and the top n words of
    // the other array are all zeroes.  "mask off" the top n words of
    // this array.
    _array.erase(_array.begin() + other._array.size(), _array.end());

  } else if (_array.size() < other._array.size() && _highest_bits) {
    // This array has fewer actual words, and the top n words of this
    // array are all ones.  "mask on" the top n words of the other
    // array.
    Array::const_iterator ai;
    for (ai = other._array.begin() + _array.size(); 
         ai != other._array.end(); 
         ++ai) {
      _array.push_back(*ai);
    }
  }

  // Consider the words that both arrays have in common.
  for (size_t i = 0; i < num_common_words; ++i) {
    _array[i] &= other._array[i];
  }

  _highest_bits &= other._highest_bits;
  normalize();
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::operator |=
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BitArray::
operator |= (const BitArray &other) {
  size_t num_common_words = min(_array.size(), other._array.size());

  // Consider the words that are on top of either array.
  if (other._array.size() < _array.size() && other._highest_bits) {
    // The other array has fewer actual words, and the top n words of
    // the other array are all ones.  The top n words of this array
    // become ones too (which means we can drop them out).
    _array.erase(_array.begin() + other._array.size(), _array.end());

  } else if (_array.size() < other._array.size() && !_highest_bits) {
    // This array has fewer actual words, and the top n words of this
    // array are all zeros.  Copy in the top n words of the other
    // array.
    Array::const_iterator ai;
    for (ai = other._array.begin() + _array.size(); 
         ai != other._array.end(); 
         ++ai) {
      _array.push_back(*ai);
    }
  }

  // Consider the words that both arrays have in common.
  for (size_t i = 0; i < num_common_words; ++i) {
    _array[i] |= other._array[i];
  }

  _highest_bits |= other._highest_bits;
  normalize();
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::operator ^=
//       Access: Published
//  Description:
////////////////////////////////////////////////////////////////////
void BitArray::
operator ^= (const BitArray &other) {
  size_t num_common_words = min(_array.size(), other._array.size());

  // Consider the words that are on top of either array.
  if (other._array.size() < _array.size() && other._highest_bits) {
    // The other array has fewer actual words, and the top n words of
    // the other array are all ones.  The top n words of this array
    // get inverted.
    Array::iterator ai;
    for (ai = _array.begin() + other._array.size(); 
         ai != _array.end(); 
         ++ai) {
      (*ai).invert_in_place();
    }

  } else if (_array.size() < other._array.size()) {
    if (!_highest_bits) {
      // This array has fewer actual words, and the top n words of this
      // array are all zeros.  Copy in the top n words of the other
      // array.
      Array::const_iterator ai;
      for (ai = other._array.begin() + _array.size(); 
           ai != other._array.end(); 
           ++ai) {
        _array.push_back(*ai);
      }
    } else {
      // This array has fewer actual words, and the top n words of this
      // array are all ones.  Copy in the top n words of the other
      // array, inverted.
      Array::const_iterator ai;
      for (ai = other._array.begin() + _array.size(); 
           ai != other._array.end(); 
           ++ai) {
        _array.push_back(~(*ai));
      }
    }
  }

  // Consider the words that both arrays have in common.
  for (size_t i = 0; i < num_common_words; ++i) {
    _array[i] ^= other._array[i];
  }

  _highest_bits ^= other._highest_bits;
  normalize();
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::operator <<=
//       Access: Published
//  Description: Logical left shift.  The rightmost bits are filled in
//               with zeroes.  Since this is an infinite bit array,
//               none of the bits on the left are lost.
////////////////////////////////////////////////////////////////////
void BitArray::
operator <<= (int shift) {
  if (shift == 0 || _array.empty()) {
    return;
  }
  if (shift < 0) {
    operator >>= (-shift);
    return;
  }

  int w = shift / num_bits_per_word;
  int b = shift % num_bits_per_word;

  if (b == 0) {
    // Easy case--word-at-a-time.
    Array new_array;
    new_array.reserve(_array.size() + w);
    for (int i = 0; i < w; ++i) {
      new_array.push_back(MaskType::all_off());
    }
    Array::const_iterator ai;
    for (ai = _array.begin(); ai != _array.end(); ++ai) {
      new_array.push_back(*ai);
    }
    _array.swap(new_array);

  } else {
    // Harder case--we have to shuffle bits between words.
    Array new_array;
    new_array.reserve(_array.size() + w + 1);
    for (int i = 0; i < w; ++i) {
      new_array.push_back(MaskType::all_off());
    }

    int downshift_count = num_bits_per_word - b;
    MaskType lower_mask = MaskType::lower_on(downshift_count);
    MaskType upper_mask = ~lower_mask;

    Array::const_iterator ai = _array.begin();
    nassertv(ai != _array.end());
    MaskType next_bits = ((*ai) & upper_mask) >> downshift_count;
    new_array.push_back(((*ai) & lower_mask) << b);
    ++ai;
    while (ai != _array.end()) {
      new_array.push_back((((*ai) & lower_mask) << b) | next_bits);
      next_bits = ((*ai) & upper_mask) >> downshift_count;
      ++ai;
    }

    // Finally, the top n bits.
    if (_highest_bits) {
      next_bits |= ~MaskType::lower_on(b);
    }
    new_array.push_back(next_bits);
    _array.swap(new_array);
  }

  normalize();
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::operator >>=
//       Access: Published
//  Description: Logical right shift.  The rightmost bits are lost.
//               Since this is an infinite bit array, there is no
//               question of sign extension; there is no need to
//               synthesize bits on the left.
////////////////////////////////////////////////////////////////////
void BitArray::
operator >>= (int shift) {
  if (shift == 0 || _array.empty()) {
    return;
  }
  if (shift < 0) {
    operator <<= (-shift);
    return;
  }

  int w = shift / num_bits_per_word;
  int b = shift % num_bits_per_word;

  if (w >= (int)_array.size()) {
    // Trivial case--shift to nothing.
    _array.clear();
    return;
  }

  if (b == 0) {
    // Easy case--word-at-a-time.
    Array new_array;
    new_array.reserve(_array.size() - w);
    Array::const_iterator ai;
    for (ai = _array.begin() + w; ai != _array.end(); ++ai) {
      new_array.push_back(*ai);
    }
    _array.swap(new_array);

  } else {
    // Harder case--we have to shuffle bits between words.
    Array new_array;
    new_array.reserve(_array.size() - w);

    int upshift_count = num_bits_per_word - b;
    MaskType lower_mask = MaskType::lower_on(b);
    MaskType upper_mask = ~lower_mask;

    Array::const_iterator ai = _array.begin() + w;
    nassertv(ai < _array.end());
    MaskType next_bits = ((*ai) & upper_mask) >> b;
    
    ++ai;
    while (ai != _array.end()) {
      new_array.push_back((((*ai) & lower_mask) << upshift_count) | next_bits);
      next_bits = ((*ai) & upper_mask) >> b;
      ++ai;
    }

    // Finally, the top n bits.
    if (_highest_bits) {
      next_bits |= ~MaskType::lower_on(upshift_count);
    }
    new_array.push_back(next_bits);
    _array.swap(new_array);
  }

  normalize();
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::generate_hash
//       Access: Public
//  Description: Adds the bitmask to the indicated hash generator.
////////////////////////////////////////////////////////////////////
void BitArray::
generate_hash(ChecksumHashGenerator &hashgen) const {
  hashgen.add_int(_highest_bits);
  Array::const_iterator ai;
  for (ai = _array.begin(); ai != _array.end(); ++ai) {
    hashgen.add_int((*ai).get_word());
  }
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::ensure_has_word
//       Access: Private
//  Description: Ensures that at least word n has been allocated into
//               the array.
////////////////////////////////////////////////////////////////////
void BitArray::
ensure_has_word(int n) {
  if (_highest_bits) {
    while (n >= (int)_array.size()) {
      _array.push_back(MaskType::all_on());
    }
  } else {
    while (n >= (int)_array.size()) {
      _array.push_back(MaskType::all_off());
    }
  }
}

////////////////////////////////////////////////////////////////////
//     Function: BitArray::normalize
//       Access: Private
//  Description: Ensures that the array is the smallest array that
//               represents this same value, by removing the topmost
//               words that are all bits off (or on).
////////////////////////////////////////////////////////////////////
void BitArray::
normalize() {
  if (_highest_bits) {
    while (!_array.empty() && _array.back() == MaskType::all_on()) {
      _array.pop_back();
    }
  } else {
    while (!_array.empty() && _array.back().is_zero()) {
      _array.pop_back();
    }
  }
}
