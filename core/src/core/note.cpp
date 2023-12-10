#include "core/note.hpp"

namespace core {

NoteInfo::NoteInfo(BookID bookID, std::string&& content)
    : NoteInfo(0, bookID, std::move(content)) {}

NoteInfo::NoteInfo(NoteID id, BookID bookID, std::string&& content)
    : id(id),
      bookID(bookID),
      content(std::move(content)) {}

NoteInfo::NoteInfo(NoteInfo&& other)
    : NoteInfo(other.id, other.bookID, std::move(other.content)) {}

BookID NoteInfo::getBookID() const { return bookID; }

std::string_view NoteInfo::getName() const {
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

const std::string& NoteInfo::getContent() const { return content; }

}  // namespace core
