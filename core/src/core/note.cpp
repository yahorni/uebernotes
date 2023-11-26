#include "core/note.hpp"

namespace core {

// NoteInfo
NoteInfo::NoteInfo(BookID bookID, std::string&& content)
    : NoteInfo(0, bookID, std::move(content)) {}

NoteInfo::NoteInfo(NoteID id, BookID bookID, std::string&& content)
    : id(id),
      bookID(bookID),
      content(std::move(content)) {}

NoteInfo::NoteInfo(NoteInfo&& other)
    : NoteInfo(other.id, other.bookID, std::move(other.content)) {}

std::string_view NoteInfo::getHeader() const {
    static const auto headerPlaceholder = "<untitled>";

    if (content.empty()) return headerPlaceholder;

    auto contentView = std::string_view(content);
    if (auto firstLineEnd = contentView.find('\n'); firstLineEnd != std::string_view::npos) {
        if (firstLineEnd == 0) return headerPlaceholder;
        return contentView.substr(0, firstLineEnd);
    }

    return contentView;
}

// Note
Note::Note(std::shared_ptr<NoteInfo> note)
    : _note(std::move(note)) {}

BookID Note::getBookID() const { return _note->bookID; }

const std::string& Note::getContent() const { return _note->content; }

}  // namespace core
