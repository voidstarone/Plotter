#include <iostream>
#include <memory>
#include "Project.h"
#include "Folder.h"
#include "Note.h"

/**
 * @brief Example demonstrating how to use the NoteTaker library
 * 
 * This example shows typical usage patterns for the NoteTaker library,
 * including creating projects, organizing folders, managing notes,
 * and using the attribute system.
 */
int main() {
    std::cout << "=== NoteTaker Library Usage Example ===" << std::endl;
    
    try {
        // 1. Create a project
        auto project = std::make_shared<Project>("Academic Research", 
            "Collection of research papers and ideas for my PhD thesis");
        
        std::cout << "\n1. Created project: " << project->getName() << std::endl;
        
        // 2. Create folder hierarchy
        auto literature = project->addFolder("Literature Review", "Academic papers and articles");
        auto ideas = project->addFolder("Research Ideas", "Original thoughts and concepts");
        auto meetings = project->addFolder("Meeting Notes", "Notes from advisor meetings");
        
        // Create nested folder structure for literature
        auto csPapers = literature->addSubfolder("Computer Science");
        auto mathPapers = literature->addSubfolder("Mathematics");
        auto aiPapers = csPapers->addSubfolder("Artificial Intelligence");
        auto mlPapers = csPapers->addSubfolder("Machine Learning");
        
        std::cout << "2. Created folder hierarchy with " << project->getFolderCount() 
                  << " top-level folders" << std::endl;
        
        // 3. Add notes with content and attributes
        std::cout << "\n3. Adding notes with attributes..." << std::endl;
        
        // AI paper note
        auto aiNote = aiPapers->addNote("Deep Learning Survey", 
            "Comprehensive survey of deep learning techniques and applications in computer vision.");
        aiNote->setAttribute<std::string>("authors", "LeCun, Y., Bengio, Y., Hinton, G.");
        aiNote->setAttribute<int>("year", 2015);
        aiNote->setAttribute<std::string>("journal", "Nature");
        aiNote->setAttribute<double>("impact_factor", 41.577);
        aiNote->setAttribute<bool>("peer_reviewed", true);
        aiNote->setAttribute<int>("citations", 35000);
        aiNote->setAttribute<std::string>("doi", "10.1038/nature14539");
        
        // ML paper note
        auto mlNote = mlPapers->addNote("Random Forest Algorithm",
            "Analysis of Random Forest algorithm performance on various datasets.");
        mlNote->setAttribute<std::string>("authors", "Breiman, L.");
        mlNote->setAttribute<int>("year", 2001);
        mlNote->setAttribute<std::string>("journal", "Machine Learning");
        mlNote->setAttribute<double>("impact_factor", 3.107);
        mlNote->setAttribute<bool>("peer_reviewed", true);
        mlNote->setAttribute<std::vector<std::string>>("keywords", {"ensemble", "classification", "regression"});
        
        // Math paper note
        auto mathNote = mathPapers->addNote("Linear Algebra Foundations",
            "Fundamental concepts in linear algebra relevant to machine learning.");
        mathNote->setAttribute<std::string>("authors", "Strang, G.");
        mathNote->setAttribute<int>("year", 2019);
        mathNote->setAttribute<std::string>("type", "textbook");
        mathNote->setAttribute<int>("pages", 574);
        mathNote->setAttribute<bool>("owns_copy", true);
        
        // Research idea note
        auto ideaNote = ideas->addNote("Novel Optimization Approach",
            "Idea for combining genetic algorithms with gradient descent for better convergence.");
        ideaNote->setAttribute<std::string>("category", "optimization");
        ideaNote->setAttribute<int>("priority", 8);
        ideaNote->setAttribute<bool>("discussed_with_advisor", false);
        ideaNote->setAttribute<std::string>("status", "draft");
        ideaNote->setAttribute<std::string>("next_steps", "Literature review on hybrid optimization");
        
        // Meeting note
        auto meetingNote = meetings->addNote("Weekly Meeting - Oct 30, 2025",
            "Discussed progress on thesis chapter 3. Need to revise methodology section.");
        meetingNote->setAttribute<std::string>("attendees", "Dr. Smith, John Doe");
        meetingNote->setAttribute<std::string>("location", "Professor's Office");
        meetingNote->setAttribute<int>("duration_minutes", 45);
        meetingNote->setAttribute<bool>("action_items_assigned", true);
        meetingNote->setAttribute<std::string>("next_meeting", "2025-11-06");
        
        // 4. Display project tree
        std::cout << "\n4. Project Structure:" << std::endl;
        std::cout << project->toTreeString() << std::endl;
        
        // 5. Demonstrate search functionality
        std::cout << "\n5. Search Examples:" << std::endl;
        
        // Find by name
        auto foundFolder = project->findFolderByName("Literature Review");
        if (foundFolder) {
            std::cout << "Found folder: " << foundFolder->getName() 
                      << " (Total notes: " << foundFolder->getTotalNoteCount() << ")" << std::endl;
        }
        
        // Deep navigation
        auto csFolder = literature->findSubfolderByName("Computer Science");
        if (csFolder) {
            auto aiFolder = csFolder->findSubfolderByName("Artificial Intelligence");
            if (aiFolder) {
                auto deepNote = aiFolder->findNoteByTitle("Deep Learning Survey");
                if (deepNote) {
                    std::cout << "Found deep note: " << deepNote->getTitle() << std::endl;
                }
            }
        }
        
        // 6. Demonstrate attribute access
        std::cout << "\n6. Attribute Examples:" << std::endl;
        
        // Safe attribute access with defaults
        std::string author = aiNote->getAttribute<std::string>("authors", "Unknown");
        int year = aiNote->getAttribute<int>("year", 0);
        double impact = aiNote->getAttribute<double>("impact_factor", 0.0);
        bool peerReviewed = aiNote->getAttribute<bool>("peer_reviewed", false);
        
        std::cout << "AI Paper - Author: " << author << ", Year: " << year 
                  << ", Impact: " << impact << ", Peer Reviewed: " << (peerReviewed ? "Yes" : "No") << std::endl;
        
        // Check attribute existence
        if (ideaNote->hasAttribute("priority")) {
            int priority = ideaNote->getAttribute<int>("priority");
            std::cout << "Research idea priority: " << priority << "/10" << std::endl;
        }
        
        // List all attributes
        auto keys = meetingNote->getAttributeKeys();
        std::cout << "Meeting note has " << keys.size() << " attributes: ";
        for (const auto& key : keys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
        
        // 7. Demonstrate modification and timestamps
        std::cout << "\n7. Timestamp Examples:" << std::endl;
        
        std::cout << "AI Note created: " << aiNote->getCreatedAtString() << std::endl;
        std::cout << "AI Note updated: " << aiNote->getUpdatedAtString() << std::endl;
        
        // Modify note to update timestamp
        aiNote->setContent(aiNote->getContent() + "\n\nUpdated with additional insights.");
        aiNote->setAttribute<std::string>("last_review", "2025-10-30");
        
        std::cout << "After modification: " << aiNote->getUpdatedAtString() << std::endl;
        
        // 8. Summary statistics
        std::cout << "\n8. Project Summary:" << std::endl;
        std::cout << "Total folders: " << project->getFolderCount() << std::endl;
        
        size_t totalNotes = 0;
        for (const auto& folder : project->getFolders()) {
            totalNotes += folder->getTotalNoteCount();
        }
        std::cout << "Total notes: " << totalNotes << std::endl;
        
        // Detailed folder statistics
        std::cout << "\nFolder Details:" << std::endl;
        std::cout << "- Literature Review: " << literature->getTotalNoteCount() << " notes total" << std::endl;
        std::cout << "- Research Ideas: " << ideas->getNoteCount() << " notes" << std::endl;
        std::cout << "- Meeting Notes: " << meetings->getNoteCount() << " notes" << std::endl;
        
        std::cout << "\n=== Library Usage Example Complete ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}