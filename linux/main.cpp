#include <iostream>

#include "linux/client.hpp"

// int main(int argc, char* argv[])
int main()
{
    linux::Client client;
    auto storage = client.getStorage();

    uebernotes::Note note(1, "note1");

    uebernotes::NormalBook book(1, "book1");
    book.addNote(note);

    storage.addNormalBook(std::move(book));

    // ---

    const auto& books = storage.getNormalBooks();
    std::cout << books.front().getName() << std::endl;

    return 0;
}
