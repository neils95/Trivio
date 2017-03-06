namespace Trivio.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class Initialize : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.Trivias",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        Text = c.String(nullable: false, maxLength: 500),
                        Upvotes = c.Int(nullable: false),
                        Downvotes = c.Int(nullable: false),
                    })
                .PrimaryKey(t => t.Id)
                .Index(t => t.Text, unique: true);
            
            CreateTable(
                "dbo.Users",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        Name = c.String(),
                        Email = c.String(nullable: false, maxLength: 50),
                        Password = c.String(nullable: false),
                        TriviaCount = c.Int(nullable: false),
                        TriviaHistoryCount = c.Int(nullable: false),
                    })
                .PrimaryKey(t => t.Id)
                .Index(t => t.Email, unique: true);
            
            CreateTable(
                "dbo.UserTriviaHistories",
                c => new
                    {
                        UserId = c.Int(nullable: false),
                        TriviaId = c.Int(nullable: false),
                        UserVote = c.Int(nullable: false),
                    })
                .PrimaryKey(t => new { t.UserId, t.TriviaId })
                .ForeignKey("dbo.Trivias", t => t.TriviaId, cascadeDelete: true)
                .ForeignKey("dbo.Users", t => t.UserId, cascadeDelete: true)
                .Index(t => t.UserId)
                .Index(t => t.TriviaId);
            
        }
        
        public override void Down()
        {
            DropForeignKey("dbo.UserTriviaHistories", "UserId", "dbo.Users");
            DropForeignKey("dbo.UserTriviaHistories", "TriviaId", "dbo.Trivias");
            DropIndex("dbo.UserTriviaHistories", new[] { "TriviaId" });
            DropIndex("dbo.UserTriviaHistories", new[] { "UserId" });
            DropIndex("dbo.Users", new[] { "Email" });
            DropIndex("dbo.Trivias", new[] { "Text" });
            DropTable("dbo.UserTriviaHistories");
            DropTable("dbo.Users");
            DropTable("dbo.Trivias");
        }
    }
}
