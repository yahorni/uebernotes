#include "core/note.hpp"

namespace core {

Note::Note(BookID bookID, std::string&& content)
    : Note(0, bookID, std::move(content)) {}

Note::Note(NoteID id, BookID bookID, std::string&& content)
    : id(id),
      bookID(bookID),
      content(std::move(content)) {}

Note::Note(Note&& other)
    : Note(other.id, other.bookID, std::move(other.content)) {}

BookID Note::getBookID() const { return bookID; }

std::string_view Note::getName() const {
    static const auto headerPlaceholder = "<untitled>";

    if (content.empty()) {
        return headerPlaceholder;
    }

    auto contentView = std::string_view(content);
    if (auto firstLineEnd = contentView.find('\n'); firstLineEnd != std::string_view::npos) {
        if (firstLineEnd == 0) {
            return headerPlaceholder;
        }
        return contentView.substr(0, firstLineEnd);
    }

    return contentView;
}

const std::string& Note::getContent() const { return content; }

}  // namespace core
