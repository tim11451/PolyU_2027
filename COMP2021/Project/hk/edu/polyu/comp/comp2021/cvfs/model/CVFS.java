package hk.edu.polyu.comp.comp2021.cvfs.model;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class CVFS {
    private static VirtualDisk virtualDisk;
    private static CriteriaManager criteriaManager ;


    // Constructors & Getter
    public CVFS() {
    }

    public static VirtualDisk getVirtualDisk() {
        return virtualDisk;
    }

    public static CriteriaManager getCriteriaManager() {
        return criteriaManager;
    }

    public void CLI(){
        Scanner scanner = new Scanner(System.in);
        Scanner sc = new Scanner(System.in);

        while(true){
            System.out.print("$ ");
            String command = sc.nextLine();
            String[] cmd = command.split(" ");
            switch(cmd[0]){
                case "checkSize":
                    System.out.println(getVirtualDisk().getRemainSize());
                    break;

                // req-1
                case "newDisk":
                    if (cmd.length != 2) {
                        System.out.println("Usage: newDisk <diskSize>");
                        break;
                    }
                    int diskSize=Integer.parseInt(cmd[1]);
                    newDisk(diskSize);
                    break;

                // req-2
                case "newDoc":
                    if (cmd.length < 4) {
                        System.out.println("Usage: newDoc <docName> <docType> <doc Content>");
                        break;
                    }
                    String docName=cmd[1];
                    String docType=cmd[2];
                    StringBuilder docContent= new StringBuilder(cmd[3]);
                    for(int i=4; i<cmd.length; i++){
                        docContent.append(" ").append(cmd[i]);
                    }
                    newDoc(docName,docType, docContent.toString());
                    break;

                // req-3
                case "newDir":
                    if (cmd.length != 2) {
                        System.out.println("Usage: newDir <dirName>");
                        break;
                    }
                    String dirName=cmd[1];
                    newDir(dirName);
                    break;

                // req-4
                case "delete":
                    if (cmd.length != 2) {
                        System.out.println("Usage: delete <fileName>");
                        break;
                    }
                    String fileName = cmd[1];
                    delete(fileName);
                    break;

                // req-5
                case "rename":
                    if (cmd.length != 3) {
                        System.out.println("Usage: rename <oldName> <newName>");
                        break;
                    }
                    String oldName=cmd[1];
                    String newName=cmd[2];
                    rename(oldName,newName);
                    break;

                // req-6
                case "changeDir":
                    if (cmd.length != 2) {
                        System.out.println("Usage: changeDir <dirName>");
                        break;
                    }
                    String cdName = cmd[1];
                    changeDir(cdName);
                    break;

                // req-7en
                case "list":
                    if (cmd.length != 1) {
                        System.out.println("Usage: list");
                        break;
                    }
                    list();
                    break;

                // req-8
                case "rList":
                    if (cmd.length != 1) {
                        System.out.println("Usage: rList");
                        break;
                    }
                    rList();
                    break;

                // req-9
                case "newSimpleCri":
                    if (cmd.length !=  5) {
                        System.out.println("Usage: newSimpleCri <criName> <attrName> <op> <val>");
                        break;
                    }
                    String criName=cmd[1];
                    String attrName=cmd[2];
                    String op=cmd[3];
                    String val=cmd[4];
                    getCriteriaManager().newSimpleCri(criName, attrName, op, val);
                    break;
                // req-11, first command
                case "newNegation":
                    if(cmd.length!=3){
                        System.out.println("Usage: newNegation <oldCri> <newCri>");
                    }
                    String oldCri = cmd[1];
                    String newCri = cmd[2];
                    getCriteriaManager().newNegation(oldCri,newCri);
                    break;
                //req-11, second command
                case "newBinaryCri":
                    if(cmd.length!=5){
                        System.out.println("Usage: newBinaryCri <newCriName> <oldCri1> <logicOp> <oldCri2>");
                    }
                    String newCriName = cmd[1];
                    String oldCri1 = cmd[2];
                    String logicOp = cmd[3];
                    String oldCri2 = cmd[4];
                    getCriteriaManager().newBinaryCri(newCriName,oldCri1,logicOp,oldCri2);
                    break;
                //req-12
                case "printAllCri":
                    if (cmd.length !=  1) {
                        System.out.println("Usage: printAllCriteria");
                        break;
                    }
                    getCriteriaManager().PrintAllSimpleCri();
                    break;
                //req-13
                case "search":
                    if (cmd.length !=  2) {
                        System.out.println("Usage: search criName");
                        break;
                    }
                    String cri = cmd[1];
                    System.out.println(search(cri));
                    break;
                case "rSearch":
                    if(cmd.length!=2){
                        System.out.println("Usage: rSearch criName");
                        break;
                    }
                    System.out.println(rSearch(cmd[1]));

                    break;
                case "save":
                    if (cmd.length !=  2) {
                        System.out.println("Usage: save <path>");
                        break;
                    }
                    String savepath=cmd[1];
                    save(savepath);
                    break;
                case "load":
                    if (cmd.length !=  2) {
                        System.out.println("Usage: save <path>");
                        break;
                    }
                    String loadpath=cmd[1];
                    load(loadpath);
                    break;
                case "quit":
                    quit();
                default:
                    System.out.println("Unknown command: " + cmd[0]);
            }
        }
    }

    // req-1
    public void newDisk(int diskSize){
        if (virtualDisk != null) {
            System.out.println("Previous disk closed.");
        }
        virtualDisk = new VirtualDisk(diskSize);
        criteriaManager = new CriteriaManager();
        System.out.println("New virtual disk with size: " + diskSize+" has been successfully created.");
    }

    // req-2
    public static void newDoc(String docName, String docType, String docContent) {
        if (virtualDisk != null) {
            if(!Valid.isValidName(docName)){
                System.out.println("Invalid document name.");
                return;
            }
            else if(!Valid.isValidType(docType)){
                System.out.println("Invalid document type.");
                return;
            }
            Document newDoc = new Document(docName,docType,docContent);
            if (newDoc.getSize() > virtualDisk.getRemainSize()) {
                System.out.println("No enough space.");
                return;
            }
            System.out.println("New document <" + docName + "> has been successfully created.");
            getVirtualDisk().getWorkingDirectory().addDoc(newDoc);
            getVirtualDisk().updateRemainSize(newDoc.getSize());
        }
        else {
            System.out.println("No virtual disk found.");
        }
    }

    // req-3
    public static void newDir(String dirName) {
        if (virtualDisk != null) {
            if(!Valid.isValidName(dirName)){
                System.out.println("Invalid directory name.");
                return;
            }
            Directory newDir = new Directory(dirName);
            if (newDir.getSize() > virtualDisk.getRemainSize()) {
                System.out.println("Not enough space.");
                return;
            }
            System.out.println("New directory <" + dirName + "> has been successfully created.");
            getVirtualDisk().getWorkingDirectory().addDir(newDir);
            getVirtualDisk().updateRemainSize(40);
        }
        else {
            System.out.println("No virtual disk found.");
        }
    }

    // req-4
    public static void delete(String fileName) {
        if (virtualDisk != null) {
            if(getVirtualDisk().getWorkingDirectory().existFile(fileName)){
                File deletedFile=virtualDisk.getWorkingDirectory().getFile(fileName);
                if(deletedFile instanceof Document) virtualDisk.getWorkingDirectory().removeDoc((Document)deletedFile);
                else  virtualDisk.getWorkingDirectory().removeDir((Directory)deletedFile);
                getVirtualDisk().updateRemainSize(-deletedFile.getSize());
                System.out.println("File <"+fileName+"> has been successfully deleted.");
            }
        }
        else {
            System.out.println("No virtual disk found.");
        }
    }

    // req-5
    public static void rename(String oldFileName, String newFileName) {
        if (virtualDisk != null) {
            if(getVirtualDisk().getWorkingDirectory().existFile(oldFileName)){
                File oldFile=virtualDisk.getWorkingDirectory().getFile(oldFileName);
                oldFile.setName(newFileName);
                System.out.println("File <" + oldFileName + "> has been renamed as <" + newFileName + ">.");
            }
        }
        else {
            System.out.println("No virtual disk found.");
        }
    }

    // req-6
    public static void changeDir(String dirName) {
        if (virtualDisk != null) {
            Directory currentDir=virtualDisk.getWorkingDirectory();
            if(dirName.equals("..")){
                if(currentDir==getVirtualDisk().getRootDirectory()){
                    System.out.println("Already in the root directory.");
                }
                else {
                    Directory parentDir = currentDir.getParentDirectory();
                    virtualDisk.setWorkingDirectory(parentDir);
                    System.out.println("Changed to directory <" + parentDir.getName() + ">");
                }
            }
            else if(currentDir.existSubDirectory(dirName)) {
                Directory newDir=currentDir.getSubDirectory(dirName);
                virtualDisk.setWorkingDirectory(newDir);
                System.out.println("Changed to directory: "+newDir.getName());
            }
        }
        else{
            System.out.println("No virtual disk found.");
        }
    }

    // req-7
    public static void list(){
        Directory currentDir = getVirtualDisk().getWorkingDirectory();
        List<File> files = currentDir.getFiles();

        int totalFiles=0;
        long totalSize=0;

        for(File file:files){
            if(file instanceof Document){
                System.out.println("File Name: "+file.getName()+", Type: "+((Document) file).getType()+", Size: "+file.getSize()+" bytes");
            }
            else{
                System.out.println("File Name: "+file.getName()+", Size: "+file.getSize()+" bytes");
            }

            totalSize += file.getSize();
            totalFiles++;
        }

        System.out.println("Total files listed: " + totalFiles);
        System.out.println("Total size: " + totalSize + " bytes.");
    }

    // req-8
    private static int rListRecur(Directory dir, int level, int[] totalFiles, long[] totalSize) {
        for(File file:dir.getFiles()) {
            if(file instanceof Document){
            System.out.println("  ".repeat(level)+"DocName: " + file.getName()+", Type: " + ((Document)file).getType() + ", Size: "+file.getSize()+" bytes");
            totalSize[level] += file.getSize();
            totalFiles[level]++;
            }else{
            System.out.println("  ".repeat(level)+"DirName: " + file.getName()+", Size: " + file.getSize()+" bytes");
            totalSize[level] += file.getSize();
            totalFiles[level]++;
            level++;
            rListRecur((Directory) file, level, totalFiles, totalSize);
            }
        }
        return level;
    }

    public static void rList(){
        Directory currentDir=virtualDisk.getWorkingDirectory();
        int totalFileNum = 0;
        int[] levelFileNum = new int[100];
        long[] levelFileSize = new long[100];
        int level = rListRecur(currentDir,0, levelFileNum , levelFileSize);
        int i;
        for(i=0;levelFileNum[i]!=0;i++){
            totalFileNum += levelFileNum[i];
        }
        System.out.println("\nTotal Files Listed: " + totalFileNum);
        System.out.println("Total Size: " + getVirtualDisk().getRootDirectory().getSize() + " bytes");

    }
    //req-13
    public static ArrayList<String> search(String criName) {
        ArrayList<String> docNamesFit = new ArrayList<>();
        for (File file : virtualDisk.getWorkingDirectory().getDocuments()) {
            if (file instanceof Document) {
                Document doc = (Document) file;
                if (criteriaManager.fitSimpleCri(doc, criName)) {
                    docNamesFit.add(doc.getName());
                }
            }
        }
        return docNamesFit;}
    //req-14
    public static ArrayList<String> rSearch(String criName) {
        ArrayList<String> docNamesFit = new ArrayList<>();

        searchRecursively(virtualDisk.getWorkingDirectory(), criName, docNamesFit);
        return docNamesFit;
    }

    private static void searchRecursively(Directory dir, String criName, ArrayList<String> docNamesFit) {
        for (File file : dir.getFiles()) {
            if (file instanceof Document) {
                Document doc = (Document) file;
                if (criteriaManager.fitSimpleCri(doc, criName)) {
                    docNamesFit.add(doc.getName());
                }
            } else if (file instanceof Directory) {
                   searchRecursively((Directory) file, criName, docNamesFit);
            }
        }
    }

    //req -15
    public void save(String path) {
        //path格式 C:\\Users\\Lenovo\\Desktop\\COMP2021\\
        if (path.endsWith("\\") || path.endsWith("/")) {
            path += "virtual_disk.txt"; // 默认文件名
        }
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(path))) {
            writer.write("Disk Size: " + getVirtualDisk().getDiskSize() + "\n");
            writer.write("Remain Size: " + getVirtualDisk().getRemainSize() + "\n");
            saveDirectory(writer, getVirtualDisk().getRootDirectory(), 0);
            System.out.println("Virtual disk saved to " + path);
        } catch (IOException e) {
            System.out.println("Error saving virtual disk.");
        }
    }


    private void saveDirectory(BufferedWriter writer, Directory dir, int level) throws IOException {
        writer.write("Directory: " + dir.getName() + "\n");
        for (File file : dir.getFiles()) {
            if (file instanceof Document) {
                Document doc = (Document) file;
                writer.write("Document: " + doc.getName() + "." + doc.getType() + "\n");
                writer.write("Content: " + doc.getContent() + "\n");
            } else if (file instanceof Directory) {
                saveDirectory(writer, (Directory) file, level + 1);
            }
        }
    }

    //req -16
    public void load(String path) {
        try (BufferedReader reader = new BufferedReader(new FileReader(path))) {
            String line;
            if (virtualDisk != null) {
                System.out.println("Previous disk closed.");
            }
            int diskSize=Integer.parseInt(reader.readLine().substring(11).trim());
            CVFS.virtualDisk = new VirtualDisk(diskSize);
            Directory currentDir = CVFS.getVirtualDisk().getRootDirectory();

            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.startsWith("Directory: ")) {
                    String currentDirName = line.substring(11);
                    if(!currentDirName.equals("root")) {
                        newDir(currentDirName);
                    }
                } else if (line.startsWith("Document:")) {
                    String[] parts = line.substring(10).trim().split("\\.");
                    String docName = parts[0];
                    String docType = parts[1];
                    line = reader.readLine(); // Read content line
                    String content = line.substring(8).trim(); // Skip "Content: "
                    newDoc(docName, docType, content);
                } else if (line.trim().isEmpty()) {
                    // If we encounter an empty line, we assume we are done with the current directory
                    currentDir = getVirtualDisk().getRootDirectory(); // Reset to root for simplicity
                }
            }
            System.out.println("Virtual disk loaded from " + path);
        } catch (IOException e) {
            System.out.println("Error loading virtual disk.");
        }
    }

    public static void quit(){
        System.out.println("Programme terminated.");
        System.exit(1);
    }
}
